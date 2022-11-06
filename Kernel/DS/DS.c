// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// Created by Jose Menta on 24/10/2022.
//
#include <DS.h>
#include "../include/DS.h"
//buenas
int64_t elemType_compare_to(void *e1, void *e2){
    //Les damos un orden descendente porque se tiende a acceder a los pid's altos a medida que se avanza
    return -(((elemType)e1)->pid-((elemType)e2)->pid);
}
uint64_t elemType_prehash(void * elem){
    return ((elemType)elem)->pid;
}