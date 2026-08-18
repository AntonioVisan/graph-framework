#ifndef NODE_H
#define NODE_H

#include <QPoint>
#include <QColor>
#include <string>

class Node
{
	int m_index;
	QPoint m_coord;
	int m_row;
	int m_column;
	QColor m_color;
	double m_longitude;
	double m_latitude;
	std::string m_name;

public:
	Node(
		int index = 0,
		QPoint p = QPoint(0, 0)
	);

	void setIndex(int index);
	void setCoord(const QPoint& p);
	void setGridPos(int row, int column);
	void setRow(int row);
	void setColumn(int column);
	void setColor(const QColor& color);
	void setLongitude(double longitude);
	void setLatitude(double latitude);
	void setGeoCoord(double longitude, double latitude);
	void setName(const std::string& name);

	int getIndex() const;
	QPoint getCoord() const;
	int getX() const;
	int getY() const;
	int getRow() const;
	int getColumn() const;
	QColor getColor() const;
	double getLongitude() const;
	double getLatitude() const;
	const std::string& getName() const;
};

#endif