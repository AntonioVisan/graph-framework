#ifndef NODE_H
#define NODE_H

#include <QPoint>

class Node
{
    int m_index;
    QPoint m_coord;

public:
    Node(int index = 0, QPoint p = QPoint(0,0));

    void setIndex(int index);
    void setCoord(const QPoint& p);

    int getIndex() const;
    QPoint getCoord() const;
    int getX() const;
    int getY() const;
};

#endif
