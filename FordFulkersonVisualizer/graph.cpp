#include "graph.h"
#include <unordered_set>
#include <QDebug>
#include <queue>
#include <stack>

Graph::Graph() {}

Graph::~Graph()
{
    for(size_t i = 0; i < m_nodes.size(); i++) {
        if(m_nodes[i]) delete m_nodes[i];
    }
}

void Graph::AddNode(QPoint pos)
{
    Node* n = new Node((int)m_nodes.size(), pos);
    m_nodes.push_back(n);
}

void Graph::AddEdge(Node* first, Node* second, int capacity) {
    m_edges[first][second] = capacity;
}

void Graph::RemoveEdge(Node *first, Node *second)
{
    m_edges[first].erase(second);
    if(m_edges[first].size()==0) m_edges.erase(first);
}

bool Graph::Bfs(Node* source, Node* sink, std::unordered_map<Node*, Node*>& parent, std::unordered_map<Node*, std::unordered_map<Node*, int>>& residualGraph) {
    std::queue<Node*> q;
    std::unordered_map<Node*, bool> visited;

    q.push(source);
    visited[source] = true;
    parent[source] = nullptr;

    while (!q.empty()) {
        Node* u = q.front();
        q.pop();

        for (auto& [v, capacity] : residualGraph[u]) {
            if (!visited[v] && capacity > 0) {
                parent[v] = u;
                visited[v] = true;
                q.push(v);

                if (v == sink) {
                    return true;
                }
            }
        }
    }

    return false;
}

std::tuple<std::unordered_map<Node *, std::unordered_map<Node *, int> >, std::unordered_map<Node *, std::unordered_map<Node *, int> >, int> Graph::FordFulkerson(Node* source, Node* sink) {
    std::unordered_map<Node*, std::unordered_map<Node*, int>> residualGraph = m_edges;
    std::unordered_map<Node *, std::unordered_map<Node *, int>> maxFlowEdges;

    std::unordered_map<Node*, Node*> parent;
    int maxFlow = 0;

    while (Bfs(source, sink, parent, residualGraph)) {
        int pathFlow = INT_MAX;
        for (Node* v = sink; v != source; v = parent[v]) {
            Node* u = parent[v];
            pathFlow = std::min(pathFlow, residualGraph[u][v]);
        }

        for (Node* v = sink; v != source; v = parent[v]) {
            Node* u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;

            if (m_edges.count(u) && m_edges[u].count(v)) {
                maxFlowEdges[u][v] += pathFlow;
            } else if (m_edges.count(v) && m_edges[v].count(u)) {
                maxFlowEdges[v][u] -= pathFlow;
            }
        }

        maxFlow += pathFlow;
    }
    return {maxFlowEdges,MinCut(source,residualGraph),maxFlow};

}


std::unordered_map<Node *, std::unordered_map<Node *, int> > Graph::MinCut(Node* source, std::unordered_map<Node*, std::unordered_map<Node*, int>>& residualGraph) {
    std::unordered_map<Node*, bool> visited;
    std::queue<Node*> q;
    std::unordered_map<Node *, std::unordered_map<Node *, int> > cutEdges;

    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        Node* u = q.front();
        q.pop();

        for (auto& [v, capacity] : residualGraph[u]) {
            if (!visited[v] && capacity > 0) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    for (auto& [u, neighbors] : m_edges) {
        for (auto& [v, capacity] : neighbors) {
            if (visited[u] && !visited[v] && capacity > 0) {
                cutEdges[u][v] = capacity;

            }
        }
    }
    return cutEdges;
}


