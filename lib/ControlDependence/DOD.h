#ifndef DG_DOD_H_
#define DG_DOD_H_

#include <map>
#include <set>

#include <dg/ADT/Queue.h>
#include <dg/ADT/SetQueue.h>
#include <dg/ADT/Bitvector.h>

#include "CDGraph.h"

namespace dg {

// compute which nodes lie on all max paths from a given node
// (for all nodes)
class AllMaxPath {
public:
    using ResultT = std::map<CDNode *, const ADT::SparseBitvector&>;

private:
    struct Info {
        ADT::SparseBitvector colors;
        unsigned short counter;
    };

    std::unordered_map<CDNode*, Info> data;

    void compute(CDGraph& graph, CDNode *target) {

        // initialize nodes
        for (auto *nd : graph) {
            data[nd].counter = nd->successors().size();
        }

        // initialize the search
        data[target].colors.set(target->getID());
        ADT::QueueLIFO<CDNode *> queue;
        queue.push(target);

        // search!
        while (!queue.empty()) {
            auto *node = queue.pop();
            assert(data[node].colors.get(target->getID()) && "A non-colored node in queue");

            for (auto *pred : node->predecessors()) {
                auto& D = data[pred];
                --D.counter;
                if (D.counter == 0) {
                    D.colors.set(target->getID());
                    queue.push(pred);
                }
            }
        }
    }

public:

    // returns control dependencies and reverse control dependencies
    ResultT compute(CDGraph& graph) {
        ResultT res;

        data.reserve(graph.size());

        for (auto *nd : graph) {
            compute(graph, nd);
            res.emplace(nd, data[nd].colors);
        }

        return res;
    }
};


class DOD {
    //using ResultT = std::map<CDNode *, std::set<std::pair<CDNode *, CDNode *>>>;
    // NOTE: although DOD is a ternary relation, we treat it as binary
    // by breaking a->(b, c) to (a, b) and (a, c). It has no effect
    // on the results of slicing.
    using ResultT = std::map<CDNode *, std::set<CDNode *>>;
    using ColoringT = ADT::SparseBitvector;

    enum class Color { WHITE, BLACK, UNCOLORED };

    struct Info {
        Color color{Color::UNCOLORED};
    };

    std::unordered_map<CDNode*, Info> data;

    CDGraph createAp(AllMaxPath::ResultT& allpaths, CDGraph& graph, CDNode *node) {
        CDGraph Ap;

        std::unordered_map<CDNode *, CDNode *> apnodes;
        std::unordered_map<CDNode *, CDNode *> revapnodes;

        // create graph
        auto it = allpaths.find(node);
        if (it == allpaths.end()) {
            return Ap;
        }

        const auto& nodes = it->second;
        for (auto *n : graph) {
            if (nodes.get(n->getID())) {
                auto *apnode = &Ap.createNode();
                apnodes[n] = apnode;
                revapnodes[apnode] = n;
                DBG(cda, "  - Ap has node " << n->getID());
            }
        }

        assert(apnodes.find(node) != apnodes.end());

        if (apnodes.size() < 3) {
            return {};
        }

        // FIXME: add edges
        // we can create a better implementation
        for (auto *n : Ap) {
            auto *gn = revapnodes[n];

            ADT::SetQueue<ADT::QueueLIFO<CDNode *>> queue;
            for (auto *s : gn->successors()) {
                queue.push(s);
            }
            while (!queue.empty()) {
                auto *cur = queue.pop();
                if (nodes.get(cur->getID())) {
                    auto * apn = apnodes[cur];
                    assert(apn);
                    n->addSuccessor(apn);
                    DBG(cda, "  - Ap edge " << gn->getID() << " -> " << cur->getID());
                } else {
                    for (auto *s : cur->successors()) {
                        queue.push(s);
                    }
                }
            }
        }

        // FIXME: color nodes

        return Ap;
    }

public:
    std::pair<ResultT, ResultT> compute(CDGraph& graph) {
        AllMaxPath allmaxpath;
        DBG_SECTION_BEGIN(cda, "Coputing nodes that are on all max paths from nodes for fun "
                                << graph.getName());
        auto allpaths = allmaxpath.compute(graph);
        DBG_SECTION_END(cda, "Done coputing nodes that are on all max paths from nodes");

        for (auto *node : graph) {

            DBG_SECTION_BEGIN(cda, "Creating Ap graph for fun " << graph.getName() <<
                                   " node " << node->getID());
            auto Ap = createAp(allpaths, graph, node);
            DBG_SECTION_END(cda, "Done creating Ap graph");
            if (Ap.empty()) {
                // no DOD possible
                continue;
            }
        }

        ResultT CD;
        ResultT revCD;

        DBG(cda, "Computing DOD");

        data.reserve(graph.size());

        return {CD, revCD};
    }
};


class DODRanganath {
    //using ResultT = std::map<CDNode *, std::set<std::pair<CDNode *, CDNode *>>>;
    // NOTE: although DOD is a ternary relation, we treat it as binary
    // by breaking a->(b, c) to (a, b) and (a, c). It has no effect
    // on the results of slicing.
    using ResultT = std::map<CDNode *, std::set<CDNode *>>;
    enum class Color { WHITE, BLACK, UNCOLORED };

    struct Info {
        Color color{Color::UNCOLORED};
    };

    std::unordered_map<CDNode*, Info> data;

    void coloredDAG(CDGraph& graph, CDNode *n, std::set<CDNode *>& visited) {
        if (visited.insert(n).second) {
            auto& successors = n->successors();
            if (successors.empty())
                return;

            for (auto *q : successors) {
                coloredDAG(graph, q, visited);
            }
            auto *s = *(successors.begin());
            auto c = data[s].color;
            for (auto *q : successors) {
                if (data[q].color != c) {
                    c = Color::UNCOLORED;
                    break;
                }
            }
            data[n].color = c;
        }
    }

    bool dependence(CDNode *n, CDNode *m, CDNode *p, CDGraph& G) {
        for (auto *n : G) {
            data[n].color = Color::UNCOLORED;
        }
        data[m].color = Color::WHITE;
        data[p].color = Color::BLACK;

        std::set<CDNode *> visited;
        visited.insert(m);
        visited.insert(p);

        coloredDAG(G, n, visited);

        bool whiteChild = false;
        bool blackChild = false;

        for (auto *q : n->successors()) {
            if (data[q].color == Color::WHITE)
                whiteChild = true;
            if (data[q].color == Color::BLACK)
                blackChild = true;
        }

        return whiteChild && blackChild;
    }

    bool reachable(CDNode *from, CDNode *n) {
        ADT::SetQueue<ADT::QueueLIFO<CDNode *>> queue;
        queue.push(from);

        while (!queue.empty()) {
            auto *cur = queue.pop();
            if (n == cur) {
                return true;
            }

            for (auto *s : cur->successors()) {
                queue.push(s);
            }
        }
        return false;
    }

public:
    std::pair<ResultT, ResultT> compute(CDGraph& graph) {
        ResultT CD;
        ResultT revCD;

        DBG(cda, "Computing DOD (Ranganath)");

        data.reserve(graph.size());

        for (auto *n : graph.predicates()) {
            for (auto *m : graph) {
                for (auto *p : graph) {
                    if (p == m) {
                        continue;
                    }

                    if (reachable(m, p) && reachable(p, m) &&
                        dependence(n, p, m, graph)) {
                        //DBG(cda, "DOD: " << n->getID() << " -> {"
                        //                 << p->getID() << ", " << m->getID() << "}");
                        CD[m].insert(n);
                        CD[p].insert(n);
                        revCD[n].insert(m);
                        revCD[n].insert(n);
                    }
                }
            }
        }


        return {CD, revCD};
    }
};

};

#endif
