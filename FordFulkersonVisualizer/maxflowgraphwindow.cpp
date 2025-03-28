#include "maxflowgraphwindow.h"
#include <QPainter>
#include <QMessageBox>

MaxFlowGraphWindow::MaxFlowGraphWindow(std::vector<Node *> &nodes, std::unordered_map<Node *, std::unordered_map<Node *, int> > edges, Node* source, Node* sink, int maxFlow, QWidget *parent) :
    QWidget{parent},
    m_nodes{nodes},
    m_edges{edges},
    m_source{source},
    m_sink{sink},
    m_maxFlow{maxFlow}
{
    setWindowTitle("Max Flow Graph");
    resize(800, 600);
    QMessageBox::information(this, "Information", "The max flow is "+ QString::number(m_maxFlow)+" .\n");
}

void MaxFlowGraphWindow::paintEvent(QPaintEvent *ev)
{

    QPainter p{this};
    std::vector<Node*> nodes = m_nodes;
    for(auto& n : nodes)
    {
        QRect r{n->GetPos().x()-m_radius,n->GetPos().y()-m_radius, m_radius*2, m_radius*2};
        if(m_source == n) {
            p.setPen(QPen(Qt::green, m_radius/10));
        }
        if(m_sink == n) {
            p.setPen(QPen(Qt::red, m_radius/10));
        }
        p.drawEllipse(r);
        p.setBrush(QBrush(Qt::NoBrush));
        if(m_sink == n || m_source == n) {
            p.setPen(Qt::white);
        }
        QString str = QString::number(n->GetValue()+1);
        p.drawText(r, Qt::AlignCenter, str);
    }
    std::unordered_map<Node*,std::unordered_map<Node*,int>> edges = m_edges;
    for(auto& [node,connections] : edges)
    {
        for(auto [currCon,cost] : connections) {
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
            p.setBrush(QBrush(Qt::white));
            p.drawPolygon(arrowTriangle);
            p.setBrush(QBrush(Qt::NoBrush));
            float xCost = (x1+x2)/2, yCost = (y1+y2)/2;
            QString costString = QString::number(cost);
            QRectF c{xCost-costString.length()*5,yCost - 10.0f, (float)costString.length()*10, 20.0f};
            p.setBrush(QBrush(Qt::white));
            p.drawRect(c);
            p.setBrush(QBrush(Qt::NoBrush));
            p.setPen(Qt::black);
            p.drawText(c, Qt::AlignCenter, costString);
            p.setPen(Qt::white);
        }
    }
}

std::pair<int, int> MaxFlowGraphWindow::calculateIntersection(int x1, int y1, int x2, int y2, int radius)
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

std::array<int, 4> MaxFlowGraphWindow::calculateTriangle(int x1, int y1, int x2, int y2, int radius)
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
