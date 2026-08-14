#ifndef NODE_H
#define NODE_H
#include <Qpoint>
class Node
{
    int m_index;
    QPoint m_coord;
public:
    Node(int index=0, QPoint p = QPoint(0,0));
    void setIndex(int index);
    void setCoord(QPoint p);
    int getIndex();
    QPoint getCoord();
    int getX();
    int getY();
};

#endif
