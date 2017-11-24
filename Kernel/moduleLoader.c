#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>

static void loadModule(uint8_t ** module, void * targetModuleAddress);
static uint32_t readUint32(uint8_t ** address);

void loadModules(void * payloadStart, void ** targetModuleAddress)
{
	int i;
	uint8_t * currentModule = (uint8_t*)payloadStart;
	uint32_t moduleCount = readUint32(&currentModule);

	for (i = 0; i < moduleCount; i++)
		loadModule(&currentModule, targetModuleAddress[i]);
}

static void loadModule(uint8_t ** module, void * targetModuleAddress)
{
	uint32_t moduleSize = readUint32(module);

	ncPrint("  Will copy module at 0x");
	ncPrintHex((uint64_t)*module);
	ncPrint(" to 0x");
	ncPrintHex((uint64_t)targetModuleAddress);
	ncPrint(" (");
	ncPrintDec(moduleSize);
	ncPrint(" bytes)");

	memcpy(targetModuleAddress, *module, moduleSize);
	*module += moduleSize;

	ncPrint(" [Done]");
	ncNewline();
}

static uint32_t readUint32(uint8_t ** address)
{
	uint32_t result = *(uint32_t*)(*address);
	*address += sizeof(uint32_t);
	return result;
}

  void mapModulesLogical(void*  physical ){
		uint64_t * PDbase= (uint64_t*) 0x10000;
		uint64_t * userEntry= PDbase + 4;
		*userEntry= (uint64_t)physical | 0x8F;// + 0x8B;
		return;

}

//in logical 0xA00000 bit present false

 void testPageFault(){
	uint64_t * PDbase= (uint64_t*) 0x10000;
	uint64_t * userEntry= PDbase + 5;
	*userEntry= 0x8E;// + 0x8B;

}

