#include "../Node/node.h"

Node::Node(int index, QPoint p)
	: m_index(index),
	m_coord(p),
	m_row(0),
	m_column(0),
	m_color(QColor()),
	m_longitude(0.0),
	m_latitude(0.0),
	m_name("")
{}

void Node::setIndex(int index)
{
	m_index = index;
}

void Node::setCoord(const QPoint& p)
{
	m_coord = p;
}

void Node::setGridPos(int row, int column)
{
	m_row = row;
	m_column = column;
}

void Node::setRow(int row)
{
	m_row = row;
}

void Node::setColumn(int column)
{
	m_column = column;
}

void Node::setColor(const QColor& color)
{
	m_color = color;
}

void Node::setLongitude(double longitude)
{
	m_longitude = longitude;
}

void Node::setLatitude(double latitude)
{
	m_latitude = latitude;
}

void Node::setGeoCoord(double longitude, double latitude)
{
	m_longitude = longitude;
	m_latitude = latitude;
}

void Node::setName(const std::string& name)
{
	m_name = name;
}

int Node::getIndex() const
{
	return m_index;
}

QPoint Node::getCoord() const
{
	return m_coord;
}

int Node::getX() const
{
	return m_coord.x();
}

int Node::getY() const
{
	return m_coord.y();
}

int Node::getRow() const
{
	return m_row;
}

int Node::getColumn() const
{
	return m_column;
}

QColor Node::getColor() const
{
	return m_color;
}

double Node::getLongitude() const
{
	return m_longitude;
}

double Node::getLatitude() const
{
	return m_latitude;
}

const std::string& Node::getName() const
{
	return m_name;
}