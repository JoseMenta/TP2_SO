
#ifndef TP2_SO_HASHADT_H
#define TP2_SO_HASHADT_H

#include <stdint.h>
#include <orderListADT.h>

#define SIZE 10

typedef TList HashCDT[SIZE];
typedef HashCDT * HashADT;

HashADT newHashADT();
void HashProcess_delete(HashADT hash, elemType myPCB);
void HashProcess_add(HashADT hash, elemType myPCB);
int HashProcess_edit(HashADT hash, elemType prevPCB, elemType newPCB);
PCB * HashProcess_get(HashADT hash, int pid);
void dump(HashADT hash);
void freeHashADT(HashADT hash);

#endif //TP2_SO_HASHADT_H
