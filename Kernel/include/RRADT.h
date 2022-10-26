//
// Created by Jose Menta on 22/10/2022.
//

#ifndef TP2_SO_ROUNDROBIN_H
#define TP2_SO_ROUNDROBIN_H
#include <RRLevelADT.h>
#include "../include/RRLevelADT.h"
#include <stdint.h>
#include <DS.h>
#include "DS.h"
#include <mm.h>
#define PRIORITY_LEVELS 5
//TODO: definir cuantos TT va a durar
#define GET_QUANTUM_INTERVAL(level) (20/(level+1))
#define CHECK_PRIORITY(priority) (priority>=0 && priority<PRIORITY_LEVELS)

typedef struct RRCDT * RRADT;

RRADT new_RR();
void free_RR(RRADT rr);
int RR_add_process(RRADT rr, elemType elem, uint64_t priority);
elemType RR_remove_process(RRADT rr, uint64_t priority,elemType elem);
elemType RR_get_next(RRADT rr);
uint64_t RR_process_count(RRADT r);
#endif //TP2_SO_ROUNDROBIN_H
