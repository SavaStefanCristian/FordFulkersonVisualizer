#ifndef MAXFLOWGRAPHWINDOW_H
#define MAXFLOWGRAPHWINDOW_H

#include <vector>
#include <unordered_map>
#include <node.h>
#include <QWidget>
#include <QPaintEvent>

class MaxFlowGraphWindow : public QWidget
{
    Q_OBJECT

public:
    MaxFlowGraphWindow(std::vector<Node *> &nodes, std::unordered_map<Node *, std::unordered_map<Node *, int> > edges, Node *source, Node *sink, int maxFlow, QWidget *parent = nullptr);
protected:

    void paintEvent(QPaintEvent* ev) override;

private:
    int m_radius = 30;
    std::vector<Node*> m_nodes;
    std::unordered_map<Node*,std::unordered_map<Node*,int>> m_edges;
    Node* m_source;
    Node* m_sink;
    int m_maxFlow;


    std::pair<int, int> calculateIntersection(int x1, int y1, int x2, int y2, int radius);
    std::array<int,4> calculateTriangle(int x1, int y1, int x2, int y2, int radius);
};

#endif // MAXFLOWGRAPHWINDOW_H
