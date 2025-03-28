#ifndef GRAPH_H
#define GRAPH_H
#pragma once

#include "node.h"
#include <vector>
#include <unordered_map>


class Graph
{
public:
    Graph();
    ~Graph();
    void AddNode(QPoint pos);
    void AddEdge(Node* first, Node* second, int cost);
    void RemoveEdge(Node* first, Node* second);
    const std::vector<Node*>& GetNodes() {return m_nodes;}
    const std::unordered_map<Node*,std::unordered_map<Node*,int>>& GetEdges() {return m_edges;}

    bool Bfs(Node* source, Node* sink, std::unordered_map<Node*, Node*>& parent, std::unordered_map<Node*, std::unordered_map<Node*, int>>& residualGraph);
    std::tuple<std::unordered_map<Node *, std::unordered_map<Node *, int> >, std::unordered_map<Node *, std::unordered_map<Node *, int> >, int> FordFulkerson(Node* source, Node* sink);
    std::unordered_map<Node *, std::unordered_map<Node *, int> > MinCut(Node *source, std::unordered_map<Node *, std::unordered_map<Node *, int> > &residualGraph);

private:
    std::vector<Node*> m_nodes;
    std::unordered_map<Node*,std::unordered_map<Node*,int>> m_edges;
};

#endif // GRAPH_H
