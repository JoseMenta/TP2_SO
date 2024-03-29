
#ifndef TP2_SO_RRLEVELADT_H
#define TP2_SO_RRLEVELADT_H
#include <stdint.h>
#include <DS.h>
typedef struct RRLevelCDT* RRLevelADT;

RRLevelADT new_RRLevel();
void free_RRLevel(RRLevelADT RRLevel);
int RRLevel_insert(RRLevelADT RRLevel,elemType process);
void RRLevel_switch_queues(RRLevelADT RRLevel);
elemType RRLevel_delete(RRLevelADT RRLevel, elemType elem);
uint32_t RRLevel_to_execute_size(RRLevelADT RRLevel);
elemType RRLevel_get_next(RRLevelADT RRLevel);
#endif //TP2_SO_RRLEVELADT_H
