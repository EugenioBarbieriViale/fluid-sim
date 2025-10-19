#ifndef SPRINGS_H
#define SPRINGS_H

#include "consts.h"

void alloc_springs(Springs*, int);
void free_springs(Springs*);
void realloc_springs(Springs*, int);

int find_spring(const Springs*, int, int);
void add_spring(Springs*, int, int, float);

void remove_spring(Springs*, int);
void shrink_springs(Springs*);

#endif
