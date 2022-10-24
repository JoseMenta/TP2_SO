//
// Created by Jose Menta on 22/10/2022.
//

#ifndef TP2_SO_ROUNDROBIN_H
#define TP2_SO_ROUNDROBIN_H
#include <RRLevel.h>
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

#endif //TP2_SO_ROUNDROBIN_H
