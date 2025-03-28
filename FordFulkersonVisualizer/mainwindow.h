#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


#include <QMouseEvent>
#include <QPaintEvent>
#include <QMessageBox>
#include "graph.h"
#include "maxflowgraphwindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mousePressEvent(QMouseEvent* m) override;
    void mouseReleaseEvent(QMouseEvent* m) override;
    void mouseMoveEvent(QMouseEvent * m) override;
    void paintEvent(QPaintEvent* ev) override;


private slots:

    void on_CapacitySelector_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    MaxFlowGraphWindow* maxFlowWindow;

    Graph m_graph;
    const int m_radius = 30;

    Node* m_selectedNode;
    int m_capacity;
    bool isFirstNodeSelected;
    bool isDragging;
    QPoint m_selectedNodePreviusCoords;

    Node* m_source;
    std::unordered_map<Node *, std::unordered_map<Node *, int> > m_minCutEdges;



    std::pair<int,int> calculateIntersection(int x1, int y1, int x2, int y2, int radius);
    std::array<int,4> calculateTriangle(int x1, int y1, int x2, int y2, int radius);

    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
