#include "common.h"

volatile int __attribute__((annotate("secret"))) sec;

struct nelem_t {
    size_t left, right_or_leafid;
    int fdim;
    float fthresh;
}; // 8*2 + 4 + 4 = 24 bytes

typedef struct nelem_t *Nodes;
typedef uint16_t *LeafIds;

#ifndef USE_TX

__attribute__((noinline))
#endif
void
preloadDataForCloak(Nodes nodes, int size) {
    uintptr_t ustart = (uintptr_t)((uintptr_t)nodes & (~lineOffMask));
    uintptr_t uend = (uintptr_t)nodes + 24 * size;
    for (; ustart < uend; ustart += 64) {
        volatile auto v = *(int *)(ustart);
    }
}

__attribute__((noinline)) void lookup_leafids(Nodes nodes, LeafIds leafids, int size) {
    size_t node = 0;
    size_t left, right;

    startTransaction();
#if !defined(NO_PRELD) && !defined(NO_INST_PRELD)
    preloadInstAddr();
#endif
    // preload data
    preloadDataForCloak(nodes, size);

start:
    while (node != -1) {
        struct nelem_t _node;
        _node.left = nodes[node].left;
        _node.right_or_leafid = nodes[node].right_or_leafid;
        _node.fdim = nodes[node].fdim;
        _node.fthresh = nodes[node].fthresh;
        left = _node.left;
        right = _node.right_or_leafid;
        // printf("node is %d\n", node);
        if (left == node) {
            leafids[0] = (uint16_t)right;
            break;
        }

        if (sec <= _node.fthresh) {
            node = left;
        } else {
            node = right;
        }
    }

end:

    endTransaction();
}

void contructTree(Nodes ns, int size) {

    // int thred = (int)pow(2, floor(log(size) / log(2)));
    // printf("thred = %d at size %d\n", thred, size);
    for (int i = 0; i < size; i++) {

        if (2 * i + 1 < size) {
            ns[i].left = 2 * i + 1;
            if (2 * i + 2 < size)
                ns[i].right_or_leafid = 2 * i + 2;
            else
                ns[i].right_or_leafid = -1;
        } else
            ns[i].left = i;

        ns[i].fdim = 0;
        ns[i].fthresh = rand() % size;
        // printf("ns[%d] = %f\n", i, ns[i].fthresh);
    }
}

int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");

    loadInput(fp);

    int *secs = new int[numSecs];
    generateSecrets(numSecs, secs, size);

    struct nelem_t nodes[size];
    // printf("nodes heads addr\n"); // lead to weird trace differences
    uint16_t leafids[1];
    contructTree(nodes, size);

    double time = 0;
    auto start_t = __parsec_roi_begin();

    for (int i = 0; i < iters; i++) {
        sec = secs[i % numSecs];
        lookup_leafids(nodes, leafids, size);
    }

    auto end_t = __parsec_roi_end();
    auto time_span = end_t - start_t;
    time += time_span;

    printCycles(time, argv[2]);
}
