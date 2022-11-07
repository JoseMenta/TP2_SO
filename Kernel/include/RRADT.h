

#ifndef TP2_SO_ROUNDROBIN_H
#define TP2_SO_ROUNDROBIN_H

#include <stdint.h>
#include <DS.h>
#define PRIORITY_LEVELS 5


#define CHECK_PRIORITY(priority) ((priority)>=0 && (priority)<PRIORITY_LEVELS)

typedef struct RRCDT * RRADT;

RRADT new_RR();
void free_RR(RRADT rr);
int RR_add_process(RRADT rr, elemType elem, uint64_t priority);
elemType RR_remove_process(RRADT rr, uint64_t priority,elemType elem);
elemType RR_get_next(RRADT rr);
uint64_t RR_process_count(RRADT r);
#endif //TP2_SO_ROUNDROBIN_H
