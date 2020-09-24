#include "common.h"

using namespace std;

volatile int __attribute__((annotate("secret"))) sec;

struct nelem_t {
    nelem_t *left, *right_or_leafid;
    int fdim;
    int fthresh;
}; // 8*2 + 4 + 4 = 24 bytes

typedef struct nelem_t *node_p;

void DFS(node_p cur) {
    volatile auto v1 = cur->fdim;
    volatile auto v2 = cur->fthresh;
    if (cur->left && cur->left != cur) {
        DFS(cur->left);
    }

    if (cur->right_or_leafid) {
        DFS(cur->right_or_leafid);
    }
}

#ifndef USE_TX

__attribute__((noinline))
#endif
int
preloadDataForCloak(node_p root, int *leaf) {
    DFS(root);
    volatile int v = *leaf;
    return v;
}
/*
void preloadDataForCloak(node_p root) {
    stack <node_p> s;
    s.push(root);
    while (!s.empty()) {
        auto cur = s.top();
        s.pop();
        volatile auto v1 = cur->fdim;
        volatile auto v2 = cur->fthresh;
        if (cur->left && cur->left != cur) {
            s.push(cur->left);
        }

        if (cur->right_or_leafid) {
            s.push(cur->right_or_leafid);
        }
    }
}*/
/*
void preloadDataForCloak(node_p root) {
    queue <node_p> que;
    que.push(root);
    while (!que.empty()) {
        auto cur = que.front();
        volatile auto v1 = cur->fdim;
        volatile auto v2 = cur->fthresh;
        que.pop();
        if (cur->left && cur->left != cur) {
            que.push(cur->left);
        }
        if (cur->right_or_leafid) {
            que.push(cur->right_or_leafid);
        }
    }
}
 */
//

__attribute__((noinline)) void lookup_leafids(node_p root, int *leaf) {
    startTransaction();
#if !defined(NO_PRELD) && !defined(NO_INST_PRELD)
    preloadInstAddr();
#endif
    preloadDataForCloak(root, leaf);

    node_p cur = root;
start:
    while (cur != NULL) {
        // printf("cur id is %u, %d\n", cur->fdim, cur->fthresh);
        if (cur->left == cur) {
            *leaf = cur->fdim;
            break;
        }
        // if (queries.item(i, _node.fdim) <= _node.fthresh) {
        if (sec <= cur->fthresh) {
            cur = cur->left;
        } else {
            cur = cur->right_or_leafid;
        }
    }
end:
    endTransaction();
}

node_p constructTree(int id, int size) {
    node_p root = (node_p)malloc(sizeof(nelem_t));
    root->fdim = id;
    root->fthresh = rand() % size;

    if (2 * id + 1 < size) {
        root->left = constructTree(2 * id + 1, size);
        if (2 * id + 2 < size)
            root->right_or_leafid = constructTree(2 * id + 2, size);
        else
            root->right_or_leafid = NULL;
    } else {
        root->left = root;
    }

    return root;
}

int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");

    loadInput(fp);

    int *secs = new int[numSecs];
    generateSecrets(numSecs, secs, size);

    // printf("nodes heads addr\n"); // lead to weird trace differences
    int leafids;
    // float queries[3][1];
    node_p nodes[size];
    node_p root = constructTree(0, size);

    double time = 0;
    auto start_t = __parsec_roi_begin();

    for (int i = 0; i < iters; i++) {
        sec = secs[i % numSecs];
        //printf("sec %d\n", sec);
        lookup_leafids(root, &leafids);
        // printf("leadid: %lu\n", &leafids);
    }

    auto end_t = __parsec_roi_end();
    auto time_span = end_t - start_t;
    time += time_span;

    printCycles(time, argv[2]);
}