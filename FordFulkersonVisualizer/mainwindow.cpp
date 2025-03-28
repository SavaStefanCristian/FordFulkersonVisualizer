#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isFirstNodeSelected = false;
    isDragging = false;
    m_capacity = 0;
    m_source = nullptr;
    maxFlowWindow = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
    if(maxFlowWindow!=nullptr){
        maxFlowWindow->close();
        delete maxFlowWindow;
        maxFlowWindow = nullptr;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *m)
{
    if(!isFirstNodeSelected) return;
    if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes = m_graph.GetNodes();
        for(Node* n : nodes) {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                < m_radius*m_radius) {
                if(isFirstNodeSelected)
                {
                    if(m_selectedNode==n) {
                        isDragging = true;
                        m_selectedNodePreviusCoords = m_selectedNode->GetPos();
                    }
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *m) {
    if(isDragging) {
        m_selectedNode->SetPos(m->pos());
        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *m)
{
    if(m->button() == Qt::MiddleButton) {
        std::vector<Node*> nodes = m_graph.GetNodes();
        for(Node* n : nodes)
        {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                < 2*2*m_radius*m_radius) {
                if(m_source==nullptr) {
                    m_source = n;
                }
                else {
                    auto fordFulkersonResult = m_graph.FordFulkerson(m_source,n);
                    auto maxFlowGraphEdges = std::get<0>(fordFulkersonResult);
                    m_minCutEdges = std::get<1>(fordFulkersonResult);
                    int maxFlow = std::get<2>(fordFulkersonResult);

                    if(maxFlowGraphEdges.empty() && maxFlow ==0) {
                        QMessageBox::information(this, "Information", "The path is not accessible.\n");
                        m_source = nullptr;
                        continue;
                    }
                    if(maxFlowWindow!=nullptr){
                        maxFlowWindow->close();
                        delete maxFlowWindow;
                        maxFlowWindow = nullptr;
                    }
                    maxFlowWindow = new MaxFlowGraphWindow{nodes,maxFlowGraphEdges,m_source,n,maxFlow};
                    maxFlowWindow->show();

                    m_source = nullptr;


                }
                update();
            }
        }
    }
    else if (m->button() == Qt::RightButton)
    {
        std::vector<Node*> nodes = m_graph.GetNodes();
        for(Node* n : nodes)
        {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                        < 2*2*m_radius*m_radius) {
                return;
            }
        }
        m_graph.AddNode(m->pos());
        m_minCutEdges.clear();
        update();
    }
    else if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes;
        if(isDragging) {
            nodes = m_graph.GetNodes();
            for(Node* n : nodes) {
                if(n==m_selectedNode) continue;
                if((n->GetPos().x() - m_selectedNode->GetPos().x())*(n->GetPos().x() - m_selectedNode->GetPos().x())+
                    (n->GetPos().y() - m_selectedNode->GetPos().y())*(n->GetPos().y() - m_selectedNode->GetPos().y())
                        < 2*2*m_radius*m_radius) {
                    m_selectedNode->SetPos(m_selectedNodePreviusCoords);
                    break;
                }
            }
            isDragging =false;
            isFirstNodeSelected = false;
            m_selectedNode = nullptr;
            update();
            return;
        }
        nodes = m_graph.GetNodes();
        std::unordered_map<Node*, std::unordered_map<Node*,int>> edges = m_graph.GetEdges();
        for(Node* n : nodes) {
            if((n->GetPos().x() - m->pos().x())*(n->GetPos().x() - m->pos().x())+
                    (n->GetPos().y() - m->pos().y())*(n->GetPos().y() - m->pos().y())
                        < m_radius*m_radius) {
                if(!isFirstNodeSelected) {

                    isFirstNodeSelected = true;
                    m_selectedNode =n;
                }
                else
                {
                    if(m_selectedNode!=n) {
                        if(edges.find(m_selectedNode)!=edges.end() && edges[m_selectedNode].find(n)!=edges[m_selectedNode].end())
                            m_graph.RemoveEdge(m_selectedNode,n);
                            else
                            m_graph.AddEdge(m_selectedNode,n, m_capacity);
                        m_minCutEdges.clear();
                    }
                    isFirstNodeSelected = false;
                    m_selectedNode = nullptr;
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    QPainter p{this};
    std::vector<Node*> nodes = m_graph.GetNodes();
    for(auto& n : nodes)
    {
        QRect r{n->GetPos().x()-m_radius,n->GetPos().y()-m_radius, m_radius*2, m_radius*2};
        if(m_selectedNode == n) {
            p.setPen(QPen(Qt::green, m_radius/10));
        }
        if(m_source == n) {
            p.setPen(QPen(Qt::yellow, m_radius/10));
        }
        p.drawEllipse(r);
        p.setBrush(QBrush(Qt::NoBrush));
        if(m_selectedNode == n || m_source == n) {
            p.setPen(Qt::white);
        }
        QString str = QString::number(n->GetValue()+1);
        p.drawText(r, Qt::AlignCenter, str);
    }
    std::unordered_map<Node*,std::unordered_map<Node*,int>> edges = m_graph.GetEdges();
    for(auto& [node,connections] : edges)
    {
        for(auto [currCon,cost] : connections) {
            Qt::GlobalColor color = Qt::white;
            if(!(m_minCutEdges.empty())) {
                if(m_minCutEdges.find(node) != m_minCutEdges.end()) {
                    if(m_minCutEdges[node].find(currCon) != m_minCutEdges[node].end())
                        color = Qt::green;
                }
            }
            p.setPen(color);
            int x1 = node->GetPos().x(), y1 = node->GetPos().y();
            int x2 = currCon->GetPos().x(), y2 = currCon->GetPos().y();
            if(edges.find(currCon)!=edges.end() && edges[currCon].find(node)!=edges[currCon].end()) {
                int dx = y2 - y1;
                int dy = x1 - x2;
                float length = std::sqrt(dx * dx + dy * dy);
                float offsetX = (dx / length) * m_radius / 3.0f;
                float offsetY = (dy / length) *  m_radius / 3.0f;

                x1 += offsetX;
                y1 += offsetY;
                x2 += offsetX;
                y2 += offsetY;
            }
            float x1final, y1final, x2final, y2final;
            std::pair<int,int> results1 = calculateIntersection(x1,y1,x2,y2,m_radius);
            x1final = results1.first;
            y1final = results1.second;
            std::pair<int,int> results2 = calculateIntersection(x2,y2,x1,y1,m_radius);
            x2final = results2.first;
            y2final = results2.second;
            p.drawLine(x1final,y1final,x2final,y2final);
            std::array<int,4> triangleEnds = calculateTriangle(x2,y2,x1,y1,m_radius*2);
            QPolygon arrowTriangle;
            arrowTriangle << QPoint(x2final, y2final)
                          << QPoint(triangleEnds[0], triangleEnds[1])
                          << QPoint(triangleEnds[2], triangleEnds[3]);
            p.setBrush(QBrush(color));
            p.drawPolygon(arrowTriangle);
            float xCost = (x1+x2)/2, yCost = (y1+y2)/2;
            QString costString = QString::number(cost);
            QRectF c{xCost-costString.length()*5,yCost - 10.0f, (float)costString.length()*10, 20.0f};
            p.setBrush(QBrush(color));
            p.drawRect(c);
            p.setBrush(QBrush(Qt::NoBrush));
            p.setPen(Qt::black);
            p.drawText(c, Qt::AlignCenter, costString);
            p.setPen(Qt::white);
        }
    }
}


std::pair<int, int> MainWindow::calculateIntersection(int x1, int y1, int x2, int y2, int radius)
{
    float dx = x2-x1;
    float dy = y2-y1;
    float length = std::sqrt(dx * dx + dy * dy);
    float offsetX = (dx/length) * m_radius;
    float offsetY = (dy/length) * m_radius;
    float x1final = x1 + offsetX;
    float y1final = y1 + offsetY;
    return std::pair<int,int>(x1final,y1final);
}

std::array<int,4> MainWindow::calculateTriangle(int x1, int y1, int x2, int y2, int radius)
{
    float x1final, y1final;
    int xfirst, yfirst, xsecond, ysecond;

    if (x1 == x2) {
        float yv1 = y1 + radius;
        float yv2 = y1 - radius;

        if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
            y1final = yv1;
        } else {
            y1final = yv2;
        }
        x1final = x1;
        yfirst = y1final;
        ysecond = y1final;
        xfirst = x1final - radius/8;
        xsecond = x1final +radius/8;
        return std::array<int,4>{xfirst,yfirst,xsecond,ysecond};
    }
    else {
        if(y1 == y2) {
            float xv1 = x1 + radius;
            float xv2 = x1 - radius;
            if (std::abs(x2 - xv1) < std::abs(x2 - xv2)) {
                x1final = xv1;
            } else {
                x1final = xv2;
            }
            yfirst = y1 + radius/8;
            ysecond = y1 - radius/8;
            return std::array<int, 4>{(int)x1final, yfirst, (int)x1final, ysecond};
        }
        else{
            float m = float(y2 - y1) / float(x2 - x1);
            float b = y1 - m * x1;

            float A = 1 + m * m;
            float B = 2 * (m * (b - y1) - x1);
            float C = (x1 * x1) + (b - y1) * (b - y1) - radius * radius;

            float delta = B * B - 4 * A * C;
            if (delta < 0) {
                return std::array<int,4>{(int)x1, (int)y1 + radius/4,(int)x1,(int)y1 - radius/4};
            }

            float xv1 = (-B + std::sqrt(delta)) / (2 * A);
            float xv2 = (-B - std::sqrt(delta)) / (2 * A);

            float yv1 = m * xv1 + b;
            float yv2 = m * xv2 + b;

            float dist1 = std::pow(x2 - xv1, 2) + std::pow(y2 - yv1, 2);
            float dist2 = std::pow(x2 - xv2, 2) + std::pow(y2 - yv2, 2);

            if (dist1 < dist2) {
                x1final = xv1;
                y1final = yv1;
            } else {
                x1final = xv2;
                y1final = yv2;
            }
            float m2 = -1/m;

            double dx = m_radius/4 / sqrt(1 + m2 * m2);
            double dy = m2 * dx;

            int xfirst = x1final + dx;
            int yfirst = y1final + dy;

            int xsecond = x1final - dx;
            int ysecond = y1final - dy;

            return std::array<int,4>{xfirst,yfirst,xsecond,ysecond};
        }
    }
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event){
    if(maxFlowWindow!=nullptr){
        maxFlowWindow->close();
        delete maxFlowWindow;
        maxFlowWindow = nullptr;
    }
    event->accept();
}



void MainWindow::on_CapacitySelector_valueChanged(int arg1)
{
    m_capacity = arg1;
}

