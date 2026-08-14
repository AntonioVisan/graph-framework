#ifndef NODE_H
#define NODE_H

#include <QPoint>
#include <QColor>

class Node
{
    int m_index;
    QPoint m_coord;
    int m_row;
    int m_column;
    QColor m_color;

public:
    Node(int index = 0, QPoint p = QPoint(0, 0));

    void setIndex(int index);
    void setCoord(const QPoint& p);
    void setGridPos(int row, int column);
    void setRow(int row);
    void setColumn(int column);
    void setColor(const QColor& color);

    int getIndex() const;
    QPoint getCoord() const;
    int getX() const;
    int getY() const;
    int getRow() const;
    int getColumn() const;
    QColor getColor() const;
};

#endif