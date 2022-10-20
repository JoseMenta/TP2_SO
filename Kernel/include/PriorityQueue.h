//
// Created by gaston on 20/10/22.
//

#ifndef TP2_SO_PRIORITYQUEUE_H
#define TP2_SO_PRIORITYQUEUE_H


#define PRIORITIES 5
#define STATES 2
#define DEFAULT_PRIORITY 2
enum States{READY=1, TERMINATED};

typedef ProrityQueueCDT* PriorityQueueADT;
typedef int elemType;
typedef listADT[PRIORITIES][STATES] PriorityQueueCDT;

PriorityQueueADT new_queueADT();
void add_process(PCB * process);
int kill_process(PCB * process);
int change_priority_process(PCB * process, u_int8_t new_priority);
int change_status_process(PCB * process);
void free_priority_queue(PriorityQueueADT queue);

#endif //TP2_SO_PRIORITYQUEUE_H
