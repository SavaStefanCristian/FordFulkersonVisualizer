#ifndef NODE_H
#define NODE_H
#pragma once
#include <QPoint>

class Node
{
public:
    Node();
    Node(int val, QPoint pos) : m_value{val}, m_pos{pos}{}
    void SetValue(int val) {m_value = val;}
    void SetPos(QPoint pos) {m_pos = pos;}
    QPoint GetPos() {return m_pos;}
    int GetValue() {return m_value;}
private:
    int m_value;
    QPoint m_pos;
};

#endif // NODE_H
