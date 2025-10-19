#include "springs.h"

void alloc_springs(Springs *sprs, int capacity) {
    sprs->is = (int*)malloc(capacity * sizeof(int));
    sprs->js = (int*)malloc(capacity * sizeof(int));
    sprs->rest_lengths = (float*)malloc(capacity * sizeof(float));
    sprs->count = 0;
    sprs->capacity = capacity;
}

void free_springs(Springs *sprs) {
    free(sprs->is);
    free(sprs->js);
    free(sprs->rest_lengths);
}

void realloc_springs(Springs *sprs, int capacity) {
    sprs->is = (int*)realloc(sprs->is, capacity * sizeof(int));
    sprs->js = (int*)realloc(sprs->js, capacity * sizeof(int));
    sprs->rest_lengths = (float*)realloc(sprs->rest_lengths, capacity * sizeof(float));
    sprs->capacity = capacity;
}

int find_spring(const Springs *sprs, int i, int j) {
    for (int idx = 0; idx < sprs->count; idx++) {
        if ((sprs->is[idx] == i && sprs->js[idx] == j) ||
            (sprs->is[idx] == j && sprs->js[idx] == i)) {
            return idx;
        }
    }
    return -1;
}

void add_spring(Springs *sprs, int i, int j, float rl) {
    sprs->is[sprs->count] = i;
    sprs->js[sprs->count] = j;

    sprs->rest_lengths[sprs->count] = rl;
    sprs->count++;

    if (sprs->count >= sprs->capacity) {
        realloc_springs(sprs, sprs->capacity * 2);
    }
}

void remove_spring(Springs *sprs, int idx) {
    if (sprs->count != 0 && sprs->count < sprs->capacity) {
        sprs->is[idx] = sprs->is[sprs->count - 1];
        sprs->js[idx] = sprs->js[sprs->count - 1];

        sprs->rest_lengths[idx] = sprs->rest_lengths[sprs->count - 1];
        sprs->count--;
    }
}

void shrink_springs(Springs *sprs) {
    if (sprs->count < sprs->capacity / 4 && sprs->capacity > MIN_CAPACITY) {
        int new_capacity = sprs->capacity / 2;
        if (new_capacity < MIN_CAPACITY) new_capacity = MIN_CAPACITY;
        realloc_springs(sprs, new_capacity);
    }
}
