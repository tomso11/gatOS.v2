#ifndef MODULELOADER_H
#define MODULELOADER_H

void loadModules(void * payloadStart, void ** moduleTargetAddress);
void mapModulesLogical(void*  physical );
void testPageFault();

#endif
