#include "mutex.h"
#include "condVariable.h"
#include "queue.h"

#define COND_LOCK_IDENTIFIER "__COND_ARRAY_LOCK__"

typedef struct {
  	char name[COND_NAME_LEN];
    uint64_t state;
    uint64_t lock_queue;
	queueADT process_queue;
} cond_variable;

static cond_variable open_conds[MAX_COND_VAR];

static int array_lock;

static int is_open(int key);
static void create_new_cond(int key, char * name);
static void sync_enqueue(cond_variable * v, Process * p);
static Process * sync_dequeue(cond_variable * v);
/* static void fill_conds_info(cond_info * c_info, int key); */

static int is_open(int key) {
	return key >= 0 && key < MAX_COND_VAR && open_conds[key].state == OPEN;
}

int initialize_conditional_variable() {
	array_lock = mutex_open(COND_LOCK_IDENTIFIER);
	return 1;
}

int cond_open(char * name) {
	int i;
	mutex_lock(array_lock);

	for(i = 0; i < MAX_COND_VAR && open_conds[i].state == OPEN; i++) {
		if (strcmp(open_conds[i].name, name) == 0) {
			mutex_unlock(array_lock);
			return i;
		}
	}

	if (i == MAX_COND_VAR) {
		mutex_unlock(array_lock);
		return MAX_COND_OPEN_ERROR;
	}

	create_new_cond(i, name);

	mutex_unlock(array_lock);

	return i;
}

static void create_new_cond(int key, char * name) {
	char aux[MUTEX_NAME_LEN];
	int digits = itoa(key, aux, 10);

	strcpy(aux+digits, COND_LOCK_IDENTIFIER);

	open_conds[key].lock_queue = mutex_open(aux);

	strcpy(open_conds[key].name, name);
	open_conds[key].state = OPEN;
	open_conds[key].process_queue = create_queue();
}


int cond_wait(int cond_key, int lock_key) {
	if (is_open(cond_key)) {
		cond_variable * v = &open_conds[cond_key];
		Process * p = getCurrentProcess();
		mutex_unlock(lock_key); /* Se desbloquea el mutex de la zona critica */
		sync_enqueue(v, p);
		//block_process(p);  /* Se bloquea hasta que otro proceso haga signal o broadcast */
		changeProcessState(p->pid, BLOCKED);
		//yield_process();
		_yield();
		mutex_lock(lock_key); /* Aseguramos que al desbloquearse, recupere mutex de zona critica */
		return 1;
	}

	return NOT_OPEN_ERROR;
}

int cond_signal(int key) {
	if (is_open(key)) {
		cond_variable * v = &open_conds[key];
		Process * p = sync_dequeue(v);
		if (p != NULL)
			//unblock_process(p);  /* Deslboquea proceso que estaba bloqueado por cond_wait */
			changeProcessState(p->pid, READY);
		return 1;
	}

	return NOT_OPEN_ERROR;
}

int cond_broadcast(int key) {
	Process * p;
	if (is_open(key)) {
		cond_variable * v = &open_conds[key];
		mutex_lock(v->lock_queue);
		while(!is_empty(v->process_queue)){
			//unblock_process(dequeue(v->process_queue));
			p=dequeue(v->process_queue);
			changeProcessState(p->pid, READY);
		}
		mutex_unlock(v->lock_queue);
		return 1;
	}

	return NOT_OPEN_ERROR;
}

int cond_close(int key) {
	if (is_open(key)) {
		cond_variable * v = &open_conds[key];
		cond_broadcast(key);
		destroy_queue(v->process_queue);
		mutex_close(v->lock_queue);
		v->state = CLOSED;
		return 1;
	}

	return NOT_OPEN_ERROR;
}

static void sync_enqueue(cond_variable * v, Process * p) {
	mutex_lock(v->lock_queue);
	enqueue(v->process_queue, p);
	mutex_unlock(v->lock_queue);
}

static Process * sync_dequeue(cond_variable * v) {
	Process * p = NULL;
	mutex_lock(v->lock_queue);
	if (!is_empty(v->process_queue))
		p = dequeue(v->process_queue);
	mutex_unlock(v->lock_queue);
	return p;
}

/*
int get_conds_info(cond_info info_array[]) {
  int i, j;

  mutex_lock(array_lock);

  for (i = j = 0; i < MAX_COND_VAR; i++) {
    if (is_open(i))
      fill_conds_info(&info_array[j++], i);
  }
  
  mutex_unlock(array_lock);

  return j;
}

static void fill_conds_info(cond_info * c_info, int key) {
  strcpy(c_info->name, open_conds[key].name);
  c_info->key = key;
}
*/