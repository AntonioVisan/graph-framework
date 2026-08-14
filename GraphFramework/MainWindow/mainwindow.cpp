#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* m)
{
	m_pressedNode = nullptr;

	if (m->button() == Qt::RightButton)
	{
		bool suprapunere = false;
		std::vector<Node*> nodes = m_graph.getNodes();
		for (auto n : nodes)
			if (abs(m->pos().x() - n->getX()) < 20 && abs(m->pos().y() - n->getY()) < 20)
			{
				suprapunere = true;
				break;
			}
		if (!suprapunere)
		{
			m_graph.addNode(m->pos());
			update();
		}
	}
	else if (m->button() == Qt::LeftButton)
	{
		std::vector<Node*> nodes = m_graph.getNodes();
		Node* selected = nullptr;
		for (auto* n : nodes)
		{
			if (abs(m->pos().x() - n->getX()) < 10 && abs(m->pos().y() - n->getY()) < 10)
			{
				selected = n;
				break;
			}
		}
		if (selected != nullptr && selected->getIndex() != 0)
		{
			if (m_firstNode != nullptr &&
				m_firstNode->getIndex() != 0 &&
				m_firstNode->getIndex() != selected->getIndex())
			{
				m_graph.addEdge(m_firstNode, selected);
				m_firstNode = nullptr;
				update();
			}
			else
			{
				m_firstNode = selected;
			}
		}
	}
}

void MainWindow::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	std::vector<Node*> nodes = m_graph.getNodes();
	for (const auto* n : nodes)
	{
		QRect r(n->getX() - 10, n->getY() - 10, 20, 20);
		p.drawEllipse(r);
		QString s = QString::number(n->getIndex());
		p.drawText(r, Qt::AlignCenter, s);
	}
	std::vector<Edge> edges = m_graph.getEdges();
	for (const auto& ed : edges)
	{
		QPoint firstNode = ed.getFirst()->getCoord();
		QPoint secondNode = ed.getSecond()->getCoord();

		p.drawLine(firstNode, secondNode);
		if (m_graph.isOriented())
			drawArrow(p, firstNode, secondNode);
	}

}

void MainWindow::mouseMoveEvent(QMouseEvent* m)
{
	if (m_pressedNode)
	{
		int margin = 10, newX = m->pos().x(), newY = m->pos().y();
		if (newX < margin) newX = margin;
		if (newX > width() - margin) newX = width() - margin;
		if (newY < margin) newY = margin;
		if (newY > height() - margin) newY = height() - margin;

		std::vector<Node*> nodes = m_graph.getNodes();
		bool suprapunere = false;
		for (const auto* n : nodes)
		{
			if (n == m_pressedNode)
				continue;

			if (abs(newX - n->getX()) < 20 && abs(newY - n->getY()) < 20)
			{
				suprapunere = true;
				break;
			}
		}
		if (!suprapunere)
		{
			m_pressedNode->setCoord(QPoint(newX, newY));
			update();
		}
	}
}
void MainWindow::mousePressEvent(QMouseEvent* m)
{
	if (m->button() == Qt::MiddleButton)
	{
		std::vector<Node*> nodes = m_graph.getNodes();
		for (auto* n : nodes)
		{
			if (abs(m->pos().x() - n->getX()) < 10 &&
				abs(m->pos().y() - n->getY()) < 10)
			{
				m_pressedNode = n;
				break;
			}
		}
	}
}
void MainWindow::drawArrow(QPainter& p, QPoint start, QPoint end)
{
	const double nodeRadius = 10.0;
	const double arrowLength = 20.0;
	const double arrowWidth = 16.0;

	QPointF dir = end - start;
	qreal length = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
	if (length == 0) return;

	QPointF unit = dir / length;

	QPointF arrowTip = end - unit * nodeRadius;

	QPointF perp(-unit.y(), unit.x());

	QPointF arrowP1 = arrowTip - unit * arrowLength + perp * (arrowWidth / 2.0);
	QPointF arrowP2 = arrowTip - unit * arrowLength - perp * (arrowWidth / 2.0);

	QPolygonF arrowHead;
	arrowHead << arrowTip << arrowP1 << arrowP2;

	p.setBrush(Qt::red);
	p.drawPolygon(arrowHead);

}

void MainWindow::on_radioButton_toggled(bool checked)
{
	if (checked)
	{
		m_graph.changeState();
		update();
	}
}
void MainWindow::on_radioButton_2_toggled(bool checked)
{
	if (checked)
	{
		m_graph.changeState();
		update();
	}
}

