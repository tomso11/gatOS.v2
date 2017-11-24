#include "stdio.h"
#include "dummy.h"
#include "naiveConsole.h"

int prodCons();
void philosophers();

int consumerID[MAX_CONSUMER+1];
int producerID[MAX_PRODUCER+1];

int consumerPid[MAX_CONSUMER+1];
int producerPid[MAX_PRODUCER+1];

int producers = 1;
int consumers = 1;

int itemMutex;
int emptyCount;
int fullCount;

int prodSleepTime = 10;
int consSleepTime = 100;

const int bufferSize = 50;

int r = 0;
int w = 0;

int bufferLength = 0;

char buffer[50];

void insertItem(char c) {
	buffer[w] = c;
	w = (w + 1) % bufferSize;
}

char removeItem() {
	char result = buffer[r];
	r = (r + 1) % bufferSize;
	return result;
}
int main(int program)
{
	switch(program)
		{
			case PROD_CONS: prodCons();
				break;
			case PHILOSOPHERS: philosophers();
					break;
			case MALLOC_TEST: break;

			case PIPE_TEST:	break;
		}
		return 0;
}


int prodCons(void) {
	//Mutexes buffer access
	itemMutex = semCreate("itemMutex", 1 );

	//Counts full buffer slots
	fullCount = semCreate("fullCount", 0);

	//Counts empty buffer slots
	emptyCount = semCreate("emptyCount", bufferSize);

	//Create processes
	consumerID[0] = 0;
	producerID[0] = 0;
	producerPid[producers-1] = createProcess(producer, "PRODUCER", &producerID[producers-1]);
	consumerPid[consumers-1] = createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);

 	control();

	//Control thread speed
	return 0;

}

int a = 10;
int i = 0;
void * producer(void * ctx) {
	int itemP;
	int fullP;
	int emptyP;

	itemP = semOpen("itemMutex");
	fullP = semOpen("fullCount");

	emptyP = semOpen("emptyCount");

	while (1) {

		sleep(prodSleepTime);
		int item = i++;
		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Producer %d producing: %d\n", NULL, a);
		//Decrement the count of empty slots in the buffer (semaphore goes down)
		//Locks when the remaining empty slots are zero
		wait(emptyP);
		wait(itemP);
		insertItem(item);
		signal(itemP);
		//Increment the count of full slots in the buffer (semaphore goes up)
		signal(fullP);
	}
}

void * consumer(void * ctx) {
	int item;
	int itemC;
	int fullC;
	int emptyC;

	itemC = semOpen("itemMutex");
	fullC = semOpen("fullCount");
	emptyC = semOpen("emptyCount");

	while (1) {
		sleep(consSleepTime);
		//Decrement the count of full slots in the buffer (semaphore goes down)
		//Locks when there is no more information in the buffer
		wait(fullC);
		wait(itemC);
		item = removeItem();
		signal(itemC);
		//Increment the count of empty slots in the buffer (semaphore goes up)
		signal(emptyC);

		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Consumer %d consuming: %d\n", NULL, a);

	}
}

void control() {
	int end = 0;

	while(!end) {
		int c = getchar();
		switch(c) {
			case 'c':
				if(consumers + 1 <= MAX_CONSUMER)
				{
					consumerID[consumers] = consumers;
					consumers++;
					consumerPid[consumers-1] = createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);
				}
				break;

			case 'p':
				if(producers + 1 <= MAX_PRODUCER)
				{
					producerID[producers] = producers;
					producers++;
					producerPid[producers-1] = createProcess(producer, "PRODUCER", &producerID[producers-1]);
				}
				break;
			case 'z':
					if(producers-1 >= 0)
					{
							removeProcess(producerPid[producers-1]);
							producers--;
					}

				break;
			case 'x':
				if(consumers-1 >= 0)
				{
					removeProcess(consumerPid[consumers-1]);
					consumers--;
				}
				break;
			case 'q':
				killAllProcesses();
				//printProcesses();
				end = 1;
				print("PASOOO");
				break;
		}
	}

}

void killAllProcesses()
{
	int i;
	for(i = 0; i < producers; i++)
	{
		removeProcess(producerPid[i]);
	}
	for(i = 0; i < consumers; i++)
	{
		removeProcess(consumerPid[i]);
	}
}


int * state;

int strlen(char * c){
	int len = 0;

	while(c[len] != 0)
		len++;

	return len;
}

int isdigit(char c){
	int ch = c - '0';
	if((ch <= 9) && (ch >= 0))
		return 1;

	return 0;
}

int isNotInteger(char * c){
    char current;
    int i = 0;
    int integer = 0;
    int length = strlen(c);

    while(i < length){
        current = *(c + i);

        if(!isdigit(current))
            return -1;

        integer = (integer * 10) + (current - '0');

        i++;
    }

    return integer;
}

int validateParameters(int cant, char *toValidate[], int * input){
    if(cant > 2){
        print("Cantidad de parametros invalido");
		putchar('\n');
		return -1;
    }

    if(cant == 1)
        *input = DEFAULTP;
    else
        *input = isNotInteger(toValidate[1]);

    if((* input) == -1){
        print("Error de parametros");
		putchar('\n');
		return -1;
    }

    if((*input) >= 100){
        print("La cantidad de filosofos no puede ser mayor o igual a 100");
		putchar('\n');
		return -1;
    }

    return 0;
}

int startSemaphores(mutex_t * mutex, sem_t * semaphores[], int cant){
	int i = 0;

	if((*mutex = getMutex("mutexp")) == ERROR)
    {
        print("AQUI");
        return -1;
    }

	while(i < cant){
        if(i < 10){
            char *  buffer  = malloc(3);

            intToString(i, buffer, 1);

            //buffer[1]=0;
            //print(buffer);
		    if((*semaphores[i] = semCreate(buffer, 0)) == ERROR)
            {
                print("AQUI2");

                ncPrintDec(i);
                return -1;
            }

        }else{
            char *  buffer  = malloc(3);
            //buffer[2]=0;
            intToString(i, buffer, 2);
		    if((*semaphores[i] = semCreate(buffer, 0)) == ERROR)
            {
                print("AQUI3");
                return -1;
            }
        }

		i++;
	}

    return 0;
}

void testP(int number, int total, sem_t * sem){
    if (state[number] == HUNGRY && state[(number + 4) % total] != EATING && state[(number + 1) % total] != EATING){
        state[number] = EATING;

        sleep(10);

        print("Philosopher ");
        putNumber(number + 1);
        print(" takes fork ");
        putNumber(((number + 4) % total) + 1);
        print(" and ");
        putNumber(number + 1);
        putchar('\n');

        print("Philosopher ");
        putNumber(number + 1);
        print(" is Eating\n");

        signal(*sem);

    }
}

void take_fork(threadArgument * arg){
    lockMutex(*(arg->mutex));

    state[arg->value] = HUNGRY;

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" is Hungry\n");

    testP(arg->value, arg->cant, arg->sems + arg->value);

    freeMutex(*(arg->mutex));
            sleep(50);
    wait(*(arg->sems + arg->value));


}

void put_fork(threadArgument * arg){
    lockMutex(*(arg->mutex));

    state[arg->value] = THINKING;

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" putting fork ");
    putNumber((arg->value + 4) % arg->cant + 1);
    print(" and ");
    putNumber(arg->value + 1);
    print(" down");
    putchar('\n');

    print("Philosopher ");
    putNumber(arg->value + 1);
    print(" is thinking");
    putchar('\n');

    testP((arg->value + 4) % arg->cant, arg->cant, arg->sems + ((arg->value + 4) % arg->cant));
    testP((arg->value + 1) % arg->cant, arg->cant, arg->sems + ((arg->value + 1) % arg->cant));

    freeMutex(*(arg->mutex));
}

void * philospher(void * args){
	threadArgument * arg = (threadArgument *)args;

    while(1){
        sleep(1);
		take_fork(arg);
        sleep(1);
        put_fork(arg);
    }
}

void createThreads(int total, sem_t * semaphores[], mutex_t * mutex){
    int i = 0;

	threadArgument * args [total];

    while(i < total){
    	args[i] = malloc(sizeof(*args[i]));

    	args[i]->sems = (sem_t*)semaphores;
    	args[i]->mutex = mutex;
    	args[i]->cant = total;
    	args[i]->value = i;

       createProcess(philospher,"PHILO"  , (void*) args[i]);
        print("Philosopher ");
        putNumber(i + 1);
        print(" is thinking");
        putchar('\n');

        i++;
    }
}

void initializeArray(sem_t * array[], int dim){
	int i;

	for(i = 0; i < dim; i++)
		array[i] = malloc(sizeof(array[i]));
}

int dPhilosphers(int number){
	mutex_t mutex;
	sem_t * semaphores[number];
    int st[number];

    state = st;

    initializeArray(semaphores, number);

    if(startSemaphores(&mutex, semaphores, number) == -1)
    {
        print("FALLO");
        return -1;
    }

    createThreads(number, semaphores, &mutex);

    while(1);
}

void philosophers(){
    int philosphers = DEFAULTP;

    /*if(validateParameters(argc, argv, &philosphers) == -1)
        return -1;
*/
    if(dPhilosphers(philosphers) == -1)
        return;

	//terminateThread();

    while(1);

    return;
}
