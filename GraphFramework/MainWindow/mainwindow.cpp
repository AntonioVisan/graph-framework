#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <cmath>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, drawGraph(false)
	, drawLabyrinth(false)
{
	ui->setupUi(this);

	// drawingArea is used by the Graph page.
	ui->drawingArea->installEventFilter(this);

	// mapPage itself is used as the drawing surface for the Map page.
	ui->mapPage->installEventFilter(this);

	ui->travelingSalesmanPage->installEventFilter(this);

	m_weightedGraph.changeState();

	// Load the map.
	m_mapGraph.loadFromXML("Data/Luxembourg_Map.xml");

	// Initialize map coordinates and KD-Tree.
	for (Node* node : m_mapGraph.getNodes())
	{
		double latitude = node->getLatitude();
		double longitude = node->getLongitude();

		if (latitude < m_minLatitude)
			m_minLatitude = latitude;

		if (latitude > m_maxLatitude)
			m_maxLatitude = latitude;

		if (longitude < m_minLongitude)
			m_minLongitude = longitude;

		if (longitude > m_maxLongitude)
			m_maxLongitude = longitude;

		m_points.emplace_back(
			node->getIndex(),
			longitude,
			latitude
		);
	}

	m_tree = new KDTree(m_points);

	// Theme 6 - Traveling Salesman Problem.
	m_tspGraph.loadFromTXT("Data/input.txt");

	if (m_tspGraph.getNodes().empty())
	{
		QMessageBox::warning(
			this,
			"Traveling Salesman Problem",
			"The TSP graph could not be loaded."
		);
	}
	else if (!m_tspGraph.isConnected())
	{
		QMessageBox::warning(
			this,
			"Traveling Salesman Problem",
			"The initial graph is not connected."
		);
	}
	else
	{
		computeBoundingBox();
	}

	m_stepTimerMST = new QTimer(this);
	m_stepTimerTSP = new QTimer(this);

	connect(
		m_stepTimerMST,
		&QTimer::timeout,
		this,
		&MainWindow::onNextMstStep
	);

	connect(
		m_stepTimerTSP,
		&QTimer::timeout,
		this,
		&MainWindow::onNextTspStep
	);

	// Navigation between application pages.
	connect(ui->actionGraph, &QAction::triggered, this, [this]()
		{
			m_currentPage = Page::Graph;

			ui->stackedWidget->setCurrentWidget(ui->graphPage);

			ui->drawingArea->update();
		});

	connect(ui->actionWeighted_Graph, &QAction::triggered, this, [this]()
		{
			m_currentPage = Page::WeightedGraph;

			ui->stackedWidget->setCurrentWidget(ui->weightedGraphPage);

			update();
		});

	connect(ui->actionLabyrinth, &QAction::triggered, this, [this]()
		{
			m_currentPage = Page::Labyrinth;

			ui->stackedWidget->setCurrentWidget(ui->labyrinthPage);

			update();
		});

	connect(ui->actionMap, &QAction::triggered, this, [this]()
		{
			m_currentPage = Page::Map;

			ui->stackedWidget->setCurrentWidget(ui->mapPage);

			ui->mapPage->update();
		});

	connect(
		ui->actionTraveling_Salesman,
		&QAction::triggered,
		this,
		[this]()
		{
			m_currentPage = Page::TravelingSalesman;

			ui->stackedWidget->setCurrentWidget(
				ui->travelingSalesmanPage
			);

			ui->travelingSalesmanPage->update();
		}
	);

	m_currentPage = Page::Graph;
	ui->stackedWidget->setCurrentWidget(ui->graphPage);
}

MainWindow::~MainWindow()
{
	delete m_tree;
	delete ui;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	// Graph drawing area.
	if (watched == ui->drawingArea)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mousePressEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseReleaseEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseMove)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseMoveEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::Paint)
		{
			QPainter painter(ui->drawingArea);

			if (m_currentPage == Page::Graph)
				drawGraphContent(painter);

			return true;
		}
	}

	// Map page.
	if (watched == ui->mapPage)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mousePressEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseReleaseEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseMove)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseMoveEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::Wheel)
		{
			QWheelEvent* wheelEvent =
				static_cast<QWheelEvent*>(event);

			wheelEvent->accept();

			if (m_currentPage == Page::Map)
			{
				if (wheelEvent->angleDelta().y() > 0)
					m_mapZoom *= 1.1;
				else
					m_mapZoom /= 1.1;

				ui->mapPage->update();
			}

			return true;
		}

		if (event->type() == QEvent::Paint)
		{
			QPainter painter(ui->mapPage);

			drawMap(painter);

			return true;
		}
	}

	// Traveling Salesman page.
	if (watched == ui->travelingSalesmanPage)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mousePressEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseReleaseEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::MouseMove)
		{
			QMouseEvent* mouseEvent =
				static_cast<QMouseEvent*>(event);

			mouseMoveEvent(mouseEvent);

			return true;
		}

		if (event->type() == QEvent::Wheel)
		{
			QWheelEvent* wheelEvent =
				static_cast<QWheelEvent*>(event);

			if (wheelEvent->angleDelta().y() > 0)
				m_tspZoom *= 1.1;
			else
				m_tspZoom /= 1.1;

			wheelEvent->accept();

			ui->travelingSalesmanPage->update();

			return true;
		}

		if (event->type() == QEvent::Paint)
		{
			QPainter painter(ui->travelingSalesmanPage);

			if (m_currentPage == Page::TravelingSalesman)
			{
				if (m_drawTravellingSalesmanProblem)
					drawTravelingSalesmanProblem(painter);
				else if (m_drawMinimumSpanningTree)
					drawMinimumSpanningTree(painter);
				else if (m_drawCompleteGraph)
					drawCompleteGraph(painter);
				else
					drawInitialGraph(painter);

				drawNodes(painter);
			}

			return true;
		}
	}

	return QMainWindow::eventFilter(watched, event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* m)
{
	// Map page.
	if (m_currentPage == Page::Map)
	{
		if (m->button() == Qt::RightButton)
			m_isDragging = false;

		return;
	}

	if (m_currentPage == Page::TravelingSalesman)
	{
		if (m->button() == Qt::RightButton)
			m_isDragging = false;

		return;
	}

	m_pressedNode = nullptr;

	// Weighted Graph page.
	if (m_currentPage == Page::WeightedGraph)
	{
		m_weightedPressedNode = nullptr;

		if (m->button() == Qt::RightButton)
		{
			bool overlap = false;

			std::vector<Node*> nodes = m_weightedGraph.getNodes();

			for (auto* n : nodes)
			{
				if (abs(m->pos().x() - n->getX()) < 20 &&
					abs(m->pos().y() - n->getY()) < 20)
				{
					overlap = true;
					break;
				}
			}

			if (!overlap)
			{
				m_weightedGraph.addNode(m->pos());
				update();
			}
		}
		else if (m->button() == Qt::LeftButton)
		{
			std::vector<Node*> nodes = m_weightedGraph.getNodes();
			Node* selected = nullptr;

			for (auto* n : nodes)
			{
				if (abs(m->pos().x() - n->getX()) < 10 &&
					abs(m->pos().y() - n->getY()) < 10)
				{
					selected = n;
					break;
				}
			}

			if (selected != nullptr)
			{
				if (m_weightedFirstNode != nullptr &&
					m_weightedFirstNode->getIndex() != selected->getIndex())
				{
					m_weightedGraph.addEdge(
						m_weightedFirstNode,
						selected
					);

					m_weightedFirstNode = nullptr;
					update();
				}
				else
				{
					m_weightedFirstNode = selected;
				}
			}
		}

		return;
	}

	// Normal Graph page.
	if (m_currentPage != Page::Graph)
		return;

	if (m_showStronglyConnectedComponents)
		return;

	int mouseX = m->pos().x();
	int mouseY = m->pos().y();

	// Right click: add a new node if there is no overlapping node.
	if (m->button() == Qt::RightButton)
	{
		bool overlap = false;

		std::vector<Node*> nodes = m_manualGraph.getNodes();

		for (auto* n : nodes)
		{
			if (abs(mouseX - n->getX()) < 20 &&
				abs(mouseY - n->getY()) < 20)
			{
				overlap = true;
				break;
			}
		}

		if (!overlap)
		{
			m_manualGraph.addNode(QPoint(mouseX, mouseY));
			ui->drawingArea->update();
		}
	}

	// Left click: select two nodes and create an edge between them.
	else if (m->button() == Qt::LeftButton)
	{
		std::vector<Node*> nodes = m_manualGraph.getNodes();
		Node* selected = nullptr;

		for (auto* n : nodes)
		{
			if (abs(mouseX - n->getX()) < 10 &&
				abs(mouseY - n->getY()) < 10)
			{
				selected = n;
				break;
			}
		}

		if (selected != nullptr)
		{
			if (m_firstNode != nullptr &&
				m_firstNode->getIndex() != selected->getIndex())
			{
				m_manualGraph.addEdge(m_firstNode, selected);
				m_firstNode = nullptr;
				ui->drawingArea->update();
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

	//Draw the weighted graph on the Weighted Graph page.
	if (m_currentPage == Page::WeightedGraph)
	{
		std::vector<Node*> nodes = m_weightedGraph.getNodes();

		for (const auto* n : nodes)
		{
			QRect r(
				n->getX() - 10,
				n->getY() - 10,
				20,
				20
			);

			QString s = QString::number(n->getIndex());

			if (n->getColor().isValid())
				p.setBrush(n->getColor());
			else
				p.setBrush(Qt::NoBrush);

			p.drawEllipse(r);
			p.drawText(r, Qt::AlignCenter, s);
		}

		std::vector<Edge> edges = m_weightedGraph.getEdges();

		for (const auto& ed : edges)
		{
			QPoint firstNode = ed.getFirst()->getCoord();
			QPoint secondNode = ed.getSecond()->getCoord();

			p.drawLine(firstNode, secondNode);
			drawArrow(p, firstNode, secondNode);
		}

		return;
	}

	// Draw the labyrinth-generated graph on the Labyrinth page.
	if (m_currentPage == Page::Labyrinth && drawGraph)
	{
		const int labyrinthOffsetX = 250;
		const int labyrinthOffsetY = 100;

		std::vector<Node*> nodes = m_labyrinthGraph.getNodes();

		for (const auto* n : nodes)
		{
			QRect r(
				n->getX() + labyrinthOffsetX - 10,
				n->getY() + labyrinthOffsetY - 10,
				20,
				20
			);

			QString s = QString::number(n->getIndex());

			if (n->getColor().isValid())
				p.setBrush(n->getColor());
			else
				p.setBrush(Qt::NoBrush);

			p.drawEllipse(r);
			p.drawText(r, Qt::AlignCenter, s);
		}

		std::vector<Edge> edges = m_labyrinthGraph.getEdges();

		for (const auto& ed : edges)
		{
			QPoint firstNode(
				ed.getFirst()->getX() + labyrinthOffsetX,
				ed.getFirst()->getY() + labyrinthOffsetY
			);

			QPoint secondNode(
				ed.getSecond()->getX() + labyrinthOffsetX,
				ed.getSecond()->getY() + labyrinthOffsetY
			);

			p.drawLine(firstNode, secondNode);
		}
	}

	// Draw the labyrinth on the Labyrinth page.
	else if (m_currentPage == Page::Labyrinth && drawLabyrinth)
	{
		const int labyrinthOffsetX = 250;
		const int labyrinthOffsetY = 100;

		for (int i = 0; i < static_cast<int>(mat.size()); i++)
		{
			for (int j = 0; j < static_cast<int>(mat[i].size()); j++)
			{
				QRect r(
					j * 50 + labyrinthOffsetX,
					i * 50 + labyrinthOffsetY,
					50,
					50
				);

				// Each value in the matrix represents a different cell type.
				switch (mat[i][j])
				{
				case 0:
					p.fillRect(r, Qt::black);
					break;

				case 1:
					p.fillRect(r, Qt::white);
					break;

				case 2:
					p.fillRect(r, Qt::red);
					break;

				case 3:
					p.fillRect(r, Qt::blue);
					break;

				case 4:
					p.fillRect(r, Qt::green);
					break;
				}

				p.setPen(Qt::darkBlue);
				p.drawRect(r);
			}
		}
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent* m)
{
	// Map page.
	if (m_currentPage == Page::Map)
	{
		if (m_isDragging)
		{
			QPointF delta =
				m->position() - m_lastMousePosition;

			m_offsetX -= delta.x();
			m_offsetY -= delta.y();

			m_lastMousePosition =
				m->position();

			ui->mapPage->update();
		}

		return;
	}

	if (m_currentPage == Page::TravelingSalesman)
	{
		if (m_isDragging)
		{
			QPointF delta =
				m->position() - m_lastMousePosition;

			m_offsetX -= delta.x();
			m_offsetY -= delta.y();

			m_lastMousePosition =
				m->position();

			ui->travelingSalesmanPage->update();
		}

		return;
	}

	// Weighted Graph page.
	if (m_currentPage == Page::WeightedGraph)
	{
		if (m_weightedPressedNode)
		{
			int margin = 10;
			int newX = m->pos().x();
			int newY = m->pos().y();

			if (newX < margin)
				newX = margin;

			if (newX > width() - margin)
				newX = width() - margin;

			if (newY < margin)
				newY = margin;

			if (newY > height() - margin)
				newY = height() - margin;

			std::vector<Node*> nodes = m_weightedGraph.getNodes();

			bool overlap = false;

			for (auto* n : nodes)
			{
				if (n == m_weightedPressedNode)
					continue;

				if (abs(newX - n->getX()) < 20 &&
					abs(newY - n->getY()) < 20)
				{
					overlap = true;
					break;
				}
			}

			if (!overlap)
			{
				m_weightedPressedNode->setCoord(
					QPoint(newX, newY)
				);

				update();
			}
		}
		return;
	}

	if (m_currentPage != Page::Graph)
		return;

	if (m_showStronglyConnectedComponents)
		return;

	if (m_pressedNode)
	{
		const int margin = 10;
		const int nodeRadius = 10;

		int mouseX = m->pos().x();
		int mouseY = m->pos().y();

		// Keep the whole node inside the drawing area.
		if (mouseX < nodeRadius + margin)
			mouseX = nodeRadius + margin;

		if (mouseX > ui->drawingArea->width() - nodeRadius - margin)
			mouseX = ui->drawingArea->width() - nodeRadius - margin;

		if (mouseY < nodeRadius + margin)
			mouseY = nodeRadius + margin;

		if (mouseY > ui->drawingArea->height() - nodeRadius - margin)
			mouseY = ui->drawingArea->height() - nodeRadius - margin;

		// Mouse coordinates are already relative to drawingArea.
		int newX = mouseX;
		int newY = mouseY;

		std::vector<Node*> nodes = m_manualGraph.getNodes();
		bool overlap = false;

		// Prevent the dragged node from overlapping another node.
		for (const auto* n : nodes)
		{
			if (n == m_pressedNode)
				continue;

			if (abs(newX - n->getX()) < 20 &&
				abs(newY - n->getY()) < 20)
			{
				overlap = true;
				break;
			}
		}

		if (!overlap)
		{
			m_pressedNode->setCoord(QPoint(newX, newY));
			ui->drawingArea->update();
		}
	}
}

void MainWindow::mousePressEvent(QMouseEvent* m)
{
	// Map page.
	if (m_currentPage == Page::Map)
	{
		if (m->button() == Qt::RightButton)
		{
			m_isDragging = true;
			m_lastMousePosition = m->position();
		}

		if (m->button() == Qt::LeftButton)
		{
			double clickLongitude =
				m_minLongitude +
				(m->position().x() + m_offsetX) /
				(ui->mapPage->width() * m_mapZoom) *
				(m_maxLongitude - m_minLongitude);

			double clickLatitude =
				m_minLatitude +
				(ui->mapPage->height() -
					(m->position().y() + m_offsetY)) /
				(ui->mapPage->height() * m_mapZoom) *
				(m_maxLatitude - m_minLatitude);

			int nearestNodeID =
				m_tree->FindNearest(
					clickLongitude,
					clickLatitude
				);

			if (m_selectedNode1 == -1)
			{
				m_selectedNode1 = nearestNodeID;
			}
			else if (m_selectedNode2 == -1)
			{
				m_selectedNode2 = nearestNodeID;

				Node* startNode = nullptr;
				Node* finalNode = nullptr;

				for (Node* node : m_mapGraph.getNodes())
				{
					if (node->getIndex() == m_selectedNode1)
						startNode = node;

					if (node->getIndex() == m_selectedNode2)
						finalNode = node;
				}

				if (startNode != nullptr && finalNode != nullptr)
				{
					m_mapGraph.dijkstra(startNode, finalNode);

					m_mapGraph.findPath(finalNode);

					std::vector<int> path =
						m_mapGraph.getCurrentPath();

					if (path.empty())
					{
						QMessageBox::critical(
							this,
							"No Path Found",
							"There is no path from the starting point to the destination point!"
						);
					}
					else
					{
						QMessageBox::information(
							this,
							"Node Selection",
							"The two points have been selected. "
							"The shortest path between the two points is now displayed."
						);
					}
				}
			}
			else
			{
				m_selectedNode1 = nearestNodeID;
				m_selectedNode2 = -1;
			}

			ui->mapPage->update();
		}

		return;
	}

	if (m_currentPage == Page::TravelingSalesman)
	{
		if (m->button() == Qt::RightButton)
		{
			m_isDragging = true;
			m_lastMousePosition = m->position();
		}

		return;
	}

	if (m_currentPage == Page::WeightedGraph)
	{
		if (m->button() == Qt::MiddleButton)
		{
			std::vector<Node*> nodes = m_weightedGraph.getNodes();

			for (auto* n : nodes)
			{
				if (abs(m->pos().x() - n->getX()) < 10 &&
					abs(m->pos().y() - n->getY()) < 10)
				{
					m_weightedPressedNode = n;
					break;
				}
			}
		}
		return;
	}

	if (m_currentPage != Page::Graph)
		return;

	if (m_showStronglyConnectedComponents)
		return;

	int mouseX = m->pos().x();
	int mouseY = m->pos().y();

	// Middle click selects a node for dragging.
	if (m->button() == Qt::MiddleButton)
	{
		std::vector<Node*> nodes = m_manualGraph.getNodes();

		for (auto* n : nodes)
		{
			if (abs(mouseX - n->getX()) < 10 &&
				abs(mouseY - n->getY()) < 10)
			{
				m_pressedNode = n;
				break;
			}
		}
	}
}

void MainWindow::drawGraphContent(QPainter& p)
{
	if (m_showStronglyConnectedComponents)
	{
		// Draw the condensed graph of strongly connected components.
		const auto components =
			m_manualGraph.getStronglyConnectedComponents();

		const auto componentEdges =
			m_manualGraph.getComponentEdges();

		if (components.empty())
			return;

		std::vector<QPoint> componentPositions;
		std::vector<int> componentRadii;

		const int centerX = ui->drawingArea->width() / 2;
		const int centerY = ui->drawingArea->height() / 2;

		const int radius =
			std::min(
				ui->drawingArea->width(),
				ui->drawingArea->height()
			) / 3;

		const int componentCount =
			static_cast<int>(components.size());

		// Position each component on a circle.
		for (int i = 0; i < componentCount; ++i)
		{
			double angle =
				2.0 * M_PI * i / componentCount;

			int x = centerX +
				static_cast<int>(radius * std::cos(angle));

			int y = centerY +
				static_cast<int>(radius * std::sin(angle));

			componentPositions.emplace_back(x, y);
		}

		// Draw the component nodes.
		for (int i = 0; i < componentCount; ++i)
		{
			QString label;

			for (int node : components[i])
			{
				label += QString::number(node);

				if (node != components[i].back())
					label += ", ";
			}

			QFontMetrics fontMetrics(p.font());

			int textWidth =
				fontMetrics.horizontalAdvance(label);

			int textHeight =
				fontMetrics.height();

			int diameter =
				std::max(textWidth, textHeight) + 30;

			int nodeRadius = diameter / 2;

			componentRadii.push_back(nodeRadius);

			QRect rect(
				componentPositions[i].x() - nodeRadius,
				componentPositions[i].y() - nodeRadius,
				diameter,
				diameter
			);

			p.setBrush(Qt::white);
			p.setPen(Qt::black);

			p.drawEllipse(rect);
			p.drawText(
				rect,
				Qt::AlignCenter,
				label
			);
		}

		// Draw edges between strongly connected components.
		for (const auto& edge : componentEdges)
		{
			int firstComponent = edge.first;
			int secondComponent = edge.second;

			if (firstComponent < 0 ||
				secondComponent < 0 ||
				firstComponent >= componentCount ||
				secondComponent >= componentCount)
			{
				continue;
			}

			QPoint start =
				componentPositions[firstComponent];

			QPoint end =
				componentPositions[secondComponent];

			p.setPen(Qt::black);
			p.setBrush(Qt::black);

			p.drawLine(start, end);

			drawArrow(
				p,
				start,
				end
			);
		}

		return;
	}

	// Draw the original graph.

	std::vector<Node*> nodes =
		m_manualGraph.getNodes();

	for (const auto* n : nodes)
	{
		QRect r(
			n->getX() - 10,
			n->getY() - 10,
			20,
			20
		);

		QString s =
			QString::number(n->getIndex());

		if (n->getColor().isValid())
			p.setBrush(n->getColor());
		else
			p.setBrush(Qt::NoBrush);

		p.drawEllipse(r);
		p.drawText(
			r,
			Qt::AlignCenter,
			s
		);
	}

	std::vector<Edge> edges =
		m_manualGraph.getEdges();

	for (const auto& ed : edges)
	{
		QPoint firstNode(
			ed.getFirst()->getX(),
			ed.getFirst()->getY()
		);

		QPoint secondNode(
			ed.getSecond()->getX(),
			ed.getSecond()->getY()
		);

		p.drawLine(firstNode, secondNode);

		if (m_manualGraph.isOriented())
			drawArrow(
				p,
				firstNode,
				secondNode
			);
	}
}

void MainWindow::drawMap(QPainter& painter)
{
	// Draw graph edges.
	for (const auto& edge : m_mapGraph.getEdges())
	{
		Node* firstNode = edge.getFirst();
		Node* secondNode = edge.getSecond();

		QPointF firstPoint = mapToWindow(
			firstNode->getLongitude(),
			firstNode->getLatitude(),
			ui->mapPage->width(),
			ui->mapPage->height(),
			m_mapZoom
		);

		QPointF secondPoint = mapToWindow(
			secondNode->getLongitude(),
			secondNode->getLatitude(),
			ui->mapPage->width(),
			ui->mapPage->height(),
			m_mapZoom
		);

		painter.drawLine(firstPoint, secondPoint);
	}

	// Draw first selected node.
	if (m_selectedNode1 != -1)
	{
		Node* node = nullptr;

		for (Node* n : m_mapGraph.getNodes())
		{
			if (n->getIndex() == m_selectedNode1)
			{
				node = n;
				break;
			}
		}

		if (node != nullptr)
		{
			QPointF point = mapToWindow(
				node->getLongitude(),
				node->getLatitude(),
				ui->mapPage->width(),
				ui->mapPage->height(),
				m_mapZoom
			);

			QPen pen(Qt::blue);
			pen.setWidth(5);
			painter.setPen(pen);

			painter.drawPoint(point);
		}
	}

	// Draw second selected node.
	if (m_selectedNode2 != -1)
	{
		Node* node = nullptr;

		for (Node* n : m_mapGraph.getNodes())
		{
			if (n->getIndex() == m_selectedNode2)
			{
				node = n;
				break;
			}
		}

		if (node != nullptr)
		{
			QPointF point = mapToWindow(
				node->getLongitude(),
				node->getLatitude(),
				ui->mapPage->width(),
				ui->mapPage->height(),
				m_mapZoom
			);

			QPen pen(Qt::red);
			pen.setWidth(5);
			painter.setPen(pen);

			painter.drawPoint(point);
		}
	}

	// Draw shortest path.
	std::vector<int> path =
		m_mapGraph.getCurrentPath();

	if (path.size() >= 2)
	{
		QPen pen(Qt::green);
		pen.setWidth(3);
		painter.setPen(pen);

		for (int i = 0;
			i < static_cast<int>(path.size()) - 1;
			++i)
		{
			Node* firstNode = nullptr;
			Node* secondNode = nullptr;

			for (Node* node : m_mapGraph.getNodes())
			{
				if (node->getIndex() == path[i])
					firstNode = node;

				if (node->getIndex() == path[i + 1])
					secondNode = node;
			}

			if (firstNode == nullptr || secondNode == nullptr)
				continue;

			QPointF firstPoint = mapToWindow(
				firstNode->getLongitude(),
				firstNode->getLatitude(),
				ui->mapPage->width(),
				ui->mapPage->height(),
				m_mapZoom
			);

			QPointF secondPoint = mapToWindow(
				secondNode->getLongitude(),
				secondNode->getLatitude(),
				ui->mapPage->width(),
				ui->mapPage->height(),
				m_mapZoom
			);

			painter.drawLine(firstPoint, secondPoint);
		}
	}
}

void MainWindow::drawArrow(QPainter& p, QPoint start, QPoint end)
{
	const double nodeRadius = 10.0;
	const double arrowLength = 20.0;
	const double arrowWidth = 16.0;

	QPointF dir = end - start;

	qreal length = std::sqrt(
		dir.x() * dir.x() +
		dir.y() * dir.y()
	);

	if (length == 0)
		return;

	QPointF unit = dir / length;

	// Position the arrow tip at the edge of the destination node.
	QPointF arrowTip = end - unit * nodeRadius;
	QPointF perp(-unit.y(), unit.x());

	QPointF arrowP1 =
		arrowTip -
		unit * arrowLength +
		perp * (arrowWidth / 2.0);

	QPointF arrowP2 =
		arrowTip -
		unit * arrowLength -
		perp * (arrowWidth / 2.0);

	QPolygonF arrowHead;

	arrowHead << arrowTip
		<< arrowP1
		<< arrowP2;

	p.setBrush(Qt::red);
	p.drawPolygon(arrowHead);
}

QPointF MainWindow::mapToWindow(
	double longitude,
	double latitude,
	int width,
	int height,
	double zoom
) const
{
	double x =
		(longitude - m_minLongitude) /
		(m_maxLongitude - m_minLongitude) *
		width * zoom -
		m_offsetX;

	double y =
		height -
		((latitude - m_minLatitude) /
			(m_maxLatitude - m_minLatitude) *
			height * zoom) -
		m_offsetY;

	return QPointF(x, y);
}

QPointF MainWindow::tspMapToWindow(
	double longitude,
	double latitude,
	int width,
	int height,
	double zoom
) const
{
	const double padding = 65.0;

	double longitudeRange =
		m_tspMaxLongitude -
		m_tspMinLongitude;

	double latitudeRange =
		m_tspMaxLatitude -
		m_tspMinLatitude;

	if (longitudeRange == 0.0)
		longitudeRange = 1.0;

	if (latitudeRange == 0.0)
		latitudeRange = 1.0;

	double x =
		(longitude - m_tspMinLongitude) /
		longitudeRange *
		(width - 2 * padding) *
		zoom +
		padding -
		m_offsetX;

	double y =
		height -
		(
			(latitude - m_tspMinLatitude) /
			latitudeRange *
			(height - 2 * padding) *
			zoom +
			padding
			) -
		m_offsetY;

	return QPointF(x, y);
}

void MainWindow::computeBoundingBox()
{
	m_tspMinLatitude =
		std::numeric_limits<double>::max();

	m_tspMaxLatitude =
		std::numeric_limits<double>::lowest();

	m_tspMinLongitude =
		std::numeric_limits<double>::max();

	m_tspMaxLongitude =
		std::numeric_limits<double>::lowest();

	for (Node* node : m_tspGraph.getNodes())
	{
		m_tspMinLatitude =
			std::min(
				m_tspMinLatitude,
				node->getLatitude()
			);

		m_tspMaxLatitude =
			std::max(
				m_tspMaxLatitude,
				node->getLatitude()
			);

		m_tspMinLongitude =
			std::min(
				m_tspMinLongitude,
				node->getLongitude()
			);

		m_tspMaxLongitude =
			std::max(
				m_tspMaxLongitude,
				node->getLongitude()
			);
	}
}

void MainWindow::showInitialGraphDistances()
{
	const auto nodes =
		m_tspGraph.getNodes();

	const auto edges =
		m_tspGraph.getEdges();

	QString message;

	message +=
		QString("Initial graph contains %1 nodes and %2 edges.\n\n")
		.arg(static_cast<int>(nodes.size()))
		.arg(m_tspGraph.getInitialEdgeCount());

	message += "Initial graph edges:\n\n";

	for (const auto& edge : edges)
	{
		int from = edge.getFrom();
		int to = edge.getTo();

		message += QString("%1 - %2 : %3 km\n")
			.arg(QString::fromStdString(
				nodes[from]->getName()
			))
			.arg(QString::fromStdString(
				nodes[to]->getName()
			))
			.arg(edge.getLength());
	}

	QMessageBox::information(
		this,
		"Initial Graph",
		message
	);
}

void MainWindow::showCompleteGraphDistances()
{
	const auto nodes =
		m_tspGraph.getNodes();

	const auto& distances =
		m_tspGraph.getDistanceMatrix();

	QString message;

	int n =
		static_cast<int>(nodes.size());

	message +=
		QString("Complete graph contains %1 nodes and %2 edges.\n\n")
		.arg(n)
		.arg(n * (n - 1) / 2);

	message += "Complete graph edges:\n\n";

	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			message +=
				QString("%1 - %2 : %3 km\n")
				.arg(QString::fromStdString(
					nodes[i]->getName()
				))
				.arg(QString::fromStdString(
					nodes[j]->getName()
				))
				.arg(QString::number(
					distances[i][j],
					'f',
					0
				));
		}
	}

	QMessageBox::information(
		this,
		"Complete Graph Distances",
		message
	);
}

void MainWindow::drawNodes(QPainter& painter)
{
	painter.setRenderHint(QPainter::Antialiasing);

	const auto nodes =
		m_tspGraph.getNodes();

	for (const auto* node : nodes)
	{
		QPointF point =
			tspMapToWindow(
				node->getLongitude(),
				node->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		painter.setPen(Qt::black);
		painter.setBrush(Qt::white);

		painter.drawEllipse(
			point,
			7,
			7
		);

		painter.drawText(
			point + QPointF(10, -10),
			QString::fromStdString(
				node->getName()
			)
		);
	}
}

void MainWindow::drawInitialGraph(QPainter& painter)
{
	painter.setRenderHint(QPainter::Antialiasing);

	const auto edges =
		m_tspGraph.getEdges();

	const auto nodes =
		m_tspGraph.getNodes();

	painter.setPen(
		QPen(Qt::black, 1)
	);

	for (const auto& edge : edges)
	{
		int from = edge.getFrom();
		int to = edge.getTo();

		QPointF first =
			tspMapToWindow(
				nodes[from]->getLongitude(),
				nodes[from]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		QPointF second =
			tspMapToWindow(
				nodes[to]->getLongitude(),
				nodes[to]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		painter.drawLine(first, second);
	}
}

void MainWindow::drawCompleteGraph(QPainter& painter)
{
	painter.setRenderHint(QPainter::Antialiasing);

	const auto& completeGraph =
		m_tspGraph.getCompleteGraph();

	const auto nodes =
		m_tspGraph.getNodes();

	painter.setPen(
		QPen(Qt::lightGray, 1)
	);

	for (int i = 0;
		i < static_cast<int>(completeGraph.size());
		++i)
	{
		for (const auto& edge : completeGraph[i])
		{
			int from = edge.getFrom();
			int to = edge.getTo();

			if (from >= to)
				continue;

			QPointF first =
				tspMapToWindow(
					nodes[from]->getLongitude(),
					nodes[from]->getLatitude(),
					ui->travelingSalesmanPage->width(),
					ui->travelingSalesmanPage->height(),
					m_tspZoom
				);

			QPointF second =
				tspMapToWindow(
					nodes[to]->getLongitude(),
					nodes[to]->getLatitude(),
					ui->travelingSalesmanPage->width(),
					ui->travelingSalesmanPage->height(),
					m_tspZoom
				);

			painter.drawLine(first, second);
		}
	}
}

void MainWindow::drawMinimumSpanningTree(QPainter& painter)
{
	drawCompleteGraph(painter);

	const auto nodes =
		m_tspGraph.getNodes();

	painter.setPen(
		QPen(Qt::red, 3)
	);

	for (int i = 0;
		i < m_currentStep &&
		i < static_cast<int>(
			m_minimumSpanningTree.size()
			);
		++i)
	{
		const Edge& edge =
			m_minimumSpanningTree[i];

		int from = edge.getFrom();
		int to = edge.getTo();

		QPointF first =
			tspMapToWindow(
				nodes[from]->getLongitude(),
				nodes[from]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		QPointF second =
			tspMapToWindow(
				nodes[to]->getLongitude(),
				nodes[to]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		painter.drawLine(first, second);
	}
}

void MainWindow::drawTravelingSalesmanProblem(
	QPainter& painter
)
{
	drawCompleteGraph(painter);

	const auto nodes =
		m_tspGraph.getNodes();

	painter.setPen(
		QPen(Qt::blue, 4)
	);

	for (int i = 0;
		i < m_currentStep &&
		i + 1 <
		static_cast<int>(
			m_travelingSalesmanProblem.size()
			);
		++i)
	{
		int from =
			m_travelingSalesmanProblem[i];

		int to =
			m_travelingSalesmanProblem[i + 1];

		QPointF first =
			tspMapToWindow(
				nodes[from]->getLongitude(),
				nodes[from]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		QPointF second =
			tspMapToWindow(
				nodes[to]->getLongitude(),
				nodes[to]->getLatitude(),
				ui->travelingSalesmanPage->width(),
				ui->travelingSalesmanPage->height(),
				m_tspZoom
			);

		painter.drawLine(first, second);
	}
}

// Switch between oriented and non-oriented graph modes.
void MainWindow::on_undirectedRadioButton_toggled(bool checked)
{
	if (checked)
	{
		if (m_showStronglyConnectedComponents)
			return;

		m_manualGraph.changeState();
		update();
	}
}

void MainWindow::on_directedRadioButton_toggled(bool checked)
{
	if (checked)
	{
		if (m_showStronglyConnectedComponents)
			return;

		m_manualGraph.changeState();
		update();
	}
}

void MainWindow::on_addEdgeWeightButton_clicked()
{
	if (m_weightedGraph.getNodes().size() < 2)
	{
		QMessageBox::information(
			this,
			"Add Edge Weight",
			"There are not enough nodes to create an edge."
		);
		return;
	}

	bool ok;

	int nodeCount =
		static_cast<int>(m_weightedGraph.getNodes().size());

	int start = QInputDialog::getInt(
		this,
		"Add Edge Weight",
		"Start node index:",
		0,
		0,
		nodeCount - 1,
		1,
		&ok
	);

	if (!ok)
		return;

	int end = QInputDialog::getInt(
		this,
		"Add Edge Weight",
		"End node index:",
		0,
		0,
		nodeCount - 1,
		1,
		&ok
	);

	if (!ok)
		return;

	int cost = QInputDialog::getInt(
		this,
		"Add Edge Weight",
		"Edge weight:",
		1,
		1,
		1000,
		1,
		&ok
	);

	if (!ok)
		return;

	std::vector<Node*> nodes = m_weightedGraph.getNodes();

	Node* first = nodes[start];
	Node* second = nodes[end];

	Matrix adjacency = m_weightedGraph.getAdjacencyMatrix();

	if (adjacency[start][end] == 0)
	{
		QMessageBox::information(
			this,
			"Add Edge Weight",
			"The specified edge does not exist."
		);
		return;
	}

	m_weightedGraph.setEdgeCost(first, second, cost);

	QMessageBox::information(
		this,
		"Add Edge Weight",
		"The edge weight was added successfully."
	);

	update();
}

void MainWindow::on_showEdgeWeightsButton_clicked()
{
	if (m_weightedGraph.getEdges().empty())
	{
		QMessageBox::information(
			this,
			"Edge Weights",
			"This graph contains no edges."
		);

		return;
	}

	QString message = "Edge weights:\n";
	bool edgeFound = false;

	for (const auto& edge : m_weightedGraph.getEdges())
	{
		int cost = edge.getCost();

		if (cost != 0)
		{
			int first = edge.getFirst()->getIndex();
			int second = edge.getSecond()->getIndex();

			message += QString(
				"Edge (%1 -> %2): %3\n"
			).arg(first).arg(second).arg(cost);

			edgeFound = true;
		}
	}

	if (!edgeFound)
		message += "\nNo edge weights have been assigned.";

	QMessageBox::information(
		this,
		"Edge Weights",
		message
	);
}

void MainWindow::on_findShortestPathsButton_clicked()
{
	if (m_weightedGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Shortest Paths",
			"This graph contains no nodes."
		);

		return;
	}

	for (const auto& edge : m_weightedGraph.getEdges())
	{
		if (edge.getCost() == 0)
		{
			QMessageBox::warning(
				this,
				"Shortest Paths",
				"All edges must have a weight before computing shortest paths."
			);

			return;
		}
	}

	if (m_weightedGraph.hasCycle())
	{
		QMessageBox::warning(
			this,
			"Shortest Paths",
			"The graph contains a cycle. "
			"Shortest paths cannot be computed using the topological-order algorithm."
		);

		return;
	}

	bool ok;

	int nodeCount =
		static_cast<int>(m_weightedGraph.getNodes().size());

	int start = QInputDialog::getInt(
		this,
		"Choose Source",
		"Source node index:",
		0,
		0,
		nodeCount - 1,
		1,
		&ok
	);

	if (!ok)
		return;

	Node* source = m_weightedGraph.getNodes()[start];

	m_weightedGraph.findShortestPaths(source);

	for (auto* node : m_weightedGraph.getNodes())
	{
		if (node == source)
			continue;

		m_weightedGraph.findPath(node);

		std::vector<int> currentPath =
			m_weightedGraph.getCurrentPath();

		QString message = QString(
			"Shortest path from %1 to %2:\n"
		).arg(source->getIndex())
			.arg(node->getIndex());

		if (currentPath.empty())
		{
			message += "No path exists.";
		}
		else
		{
			for (int i = 0;
				i < static_cast<int>(currentPath.size());
				++i)
			{
				message += QString::number(currentPath[i]);

				if (i != static_cast<int>(currentPath.size()) - 1)
					message += " -> ";
			}
		}

		QMessageBox::information(
			this,
			"Shortest Path",
			message
		);

		m_weightedGraph.resetNodeColors();
		update();
	}
}

void MainWindow::on_topologicalSortButton_clicked()
{
	if (m_weightedGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Topological Sort",
			"This graph contains no nodes."
		);

		return;
	}

	if (m_weightedGraph.hasCycle())
	{
		QMessageBox::warning(
			this,
			"Topological Sort",
			"The graph contains a cycle. "
			"Topological sorting is not possible."
		);

		return;
	}

	m_weightedGraph.topologicalSort();

	QString message = "Topological order:\n";

	for (int node : m_weightedGraph.getTopologicalOrder())
	{
		message += QString::number(node);
		message += " ";
	}

	QMessageBox::information(
		this,
		"Topological Sort",
		message
	);
}

void MainWindow::on_openFileButton_clicked()
{
	// Open a file dialog and select the labyrinth input file.
	fileName = QFileDialog::getOpenFileName(
		this,
		"Select a file",
		"",
		"Text Files (*.txt)"
	);

	if (!fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"The file has been selected."
		);

		drawGraph = false;
		drawLabyrinth = false;
	}
}

void MainWindow::on_findGraphPathsButton_clicked()
{
	// Run BFS and display the path to each exit node.
	if (fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"No file has been selected."
		);
		return;
	}

	if (drawLabyrinth)
	{
		QMessageBox::information(
			this,
			"Path",
			"Labyrinth mode is currently selected."
		);
		return;
	}

	if (!drawGraph)
	{
		QMessageBox::information(
			this,
			"Graph Mode",
			"Graph mode has not been selected."
		);
		return;
	}

	m_labyrinthGraph.breadthFirstSearch();

	int pathIndex = 0;

	for (Node* exit : m_labyrinthGraph.getExitNodes())
	{
		QString title = "Path";

		QString message = QString(
			"Displaying the path %1:"
		).arg(pathIndex++);

		QMessageBox::information(
			this,
			title,
			message
		);

		m_labyrinthGraph.findPath(exit);

		update();

		QMessageBox::information(
			this,
			title,
			"Resetting the colors of the traversed nodes."
		);

		m_labyrinthGraph.resetNodeColors();

		update();
	}
}

void MainWindow::on_drawLabyrinthButton_clicked()
{
	if (fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"No file has been selected."
		);
		return;
	}

	if (drawGraph)
	{
		QMessageBox::information(
			this,
			"Draw Labyrinth",
			"Graph mode is currently selected."
		);
		return;
	}

	if (!mat.empty())
	{
		QMessageBox::information(
			this,
			"Draw Labyrinth",
			"The labyrinth has already been drawn."
		);
		return;
	}

	drawGraph = false;
	drawLabyrinth = true;

	std::string fileLabyrinth = fileName.toStdString();

	m_labyrinthGraph.readLabyrinth(mat, fileLabyrinth);

	update();
}

void MainWindow::on_drawGraphButton_clicked()
{
	if (fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"No file has been selected."
		);
		return;
	}

	if (drawLabyrinth)
	{
		QMessageBox::information(
			this,
			"Draw Graph",
			"Labyrinth mode is currently selected."
		);
		return;
	}

	if (!mat.empty())
	{
		QMessageBox::information(
			this,
			"Draw Graph",
			"The graph has already been drawn."
		);
		return;
	}

	drawGraph = true;
	drawLabyrinth = false;

	std::string fileLabyrinth = fileName.toStdString();

	m_labyrinthGraph.readLabyrinth(mat, fileLabyrinth);
	m_labyrinthGraph.constructLabyrinth(mat);

	update();
}

void MainWindow::on_clearDrawingButton_clicked()
{
	// Reset the graph, labyrinth and current file state.
	drawGraph = false;
	drawLabyrinth = false;

	m_labyrinthGraph.clearResources();

	mat.clear();

	fileName.clear();

	update();
}

void MainWindow::on_findLabyrinthPathsButton_clicked()
{
	if (fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"No file has been selected."
		);
		return;
	}

	if (drawGraph)
	{
		QMessageBox::information(
			this,
			"Draw Labyrinth",
			"Graph mode is currently selected."
		);
		return;
	}

	if (mat.empty())
	{
		QMessageBox::information(
			this,
			"Draw Exit Paths",
			"The labyrinth has not been drawn."
		);
		return;
	}

	// Reconstruct the graph and run BFS from the starting node.
	m_labyrinthGraph.constructLabyrinth(mat);
	m_labyrinthGraph.breadthFirstSearch();

	// Find and mark the path to every exit node.
	for (Node* exit : m_labyrinthGraph.getExitNodes())
	{
		m_labyrinthGraph.findPath(exit);

		std::vector<int> path = m_labyrinthGraph.getCurrentPath();

		for (int idx : path)
		{
			Node* n = m_labyrinthGraph.getNodes()[idx];

			if (mat[n->getRow()][n->getColumn()] != 2 &&
				mat[n->getRow()][n->getColumn()] != 3)
			{
				mat[n->getRow()][n->getColumn()] = 4;
			}
		}
	}

	update();
}

void MainWindow::on_clearPathsButton_clicked()
{
	if (fileName.isEmpty())
	{
		QMessageBox::information(
			this,
			"File Selection",
			"No file has been selected."
		);
		return;
	}

	if (mat.empty())
	{
		QMessageBox::information(
			this,
			"Draw Exit Paths",
			"The labyrinth has not been drawn."
		);
		return;
	}

	if (drawGraph)
	{
		QMessageBox::information(
			this,
			"Draw Labyrinth",
			"Graph mode is currently selected."
		);
		return;
	}

	// Reset all path cells back to normal walkable cells.
	for (int i = 0; i < static_cast<int>(mat.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(mat[i].size()); j++)
		{
			if (mat[i][j] == 4)
				mat[i][j] = 1;
		}
	}

	update();
}

void MainWindow::on_showInitialGraphButton_clicked()
{
	if (m_tspGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Initial Graph",
			"The graph contains no nodes."
		);

		return;
	}

	m_stepTimerMST->stop();
	m_stepTimerTSP->stop();

	m_drawInitialGraph = true;
	m_drawCompleteGraph = false;
	m_drawMinimumSpanningTree = false;
	m_drawTravellingSalesmanProblem = false;

	ui->travelingSalesmanPage->update();

	showInitialGraphDistances();
}

void MainWindow::on_showCompleteGraphButton_clicked()
{
	if (m_tspGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Complete Graph",
			"The graph contains no nodes."
		);

		return;
	}

	m_stepTimerMST->stop();
	m_stepTimerTSP->stop();

	m_tspGraph.floydWarshall();
	m_tspGraph.buildCompleteGraphFromDistances();

	m_drawInitialGraph = false;
	m_drawCompleteGraph = true;
	m_drawMinimumSpanningTree = false;
	m_drawTravellingSalesmanProblem = false;

	ui->travelingSalesmanPage->update();

	showCompleteGraphDistances();
}

void MainWindow::on_showMinimumSpanningTreeButton_clicked()
{
	if (m_tspGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Minimum Spanning Tree",
			"The graph contains no nodes."
		);

		return;
	}

	if (!m_tspGraph.isConnected())
	{
		QMessageBox::warning(
			this,
			"Minimum Spanning Tree",
			"The graph is not connected."
		);

		return;
	}

	m_stepTimerMST->stop();
	m_stepTimerTSP->stop();

	m_tspGraph.floydWarshall();
	m_tspGraph.buildCompleteGraphFromDistances();

	m_minimumSpanningTree =
		m_tspGraph.kruskal();

	m_currentStep = 0;
	m_totalDistance = 0.0;

	m_drawInitialGraph = false;
	m_drawCompleteGraph = false;
	m_drawMinimumSpanningTree = true;
	m_drawTravellingSalesmanProblem = false;

	if (m_fout.device() == nullptr)
	{
		m_file.setFileName("output.txt");

		if (m_file.open(
			QIODevice::WriteOnly |
			QIODevice::Text))
		{
			m_fout.setDevice(&m_file);
		}
	}

	if (m_fout.device() != nullptr)
	{
		m_fout
			<< "Minimum Spanning Tree construction\n\n";
	}

	ui->travelingSalesmanPage->update();

	onNextMstStep();
}

void MainWindow::onNextMstStep()
{
	if (m_currentStep >=
		static_cast<int>(
			m_minimumSpanningTree.size()
			))
	{
		m_stepTimerMST->stop();

		QMessageBox::information(
			this,
			"Minimum Spanning Tree",
			QString(
				"The minimum spanning tree has been generated successfully.\n\n"
				"Total distance: %1 km"
			).arg(m_totalDistance)
		);

		if (m_fout.device() != nullptr)
		{
			m_fout
				<< "Minimum Spanning Tree completed.\n"
				<< "Total distance: "
				<< m_totalDistance
				<< " km.\n\n";
		}

		return;
	}

	const Edge& edge =
		m_minimumSpanningTree[m_currentStep];

	int from = edge.getFrom();
	int to = edge.getTo();

	double distance =
		m_tspGraph.getDistanceMatrix()[from][to];

	m_totalDistance += distance;

	++m_currentStep;

	ui->travelingSalesmanPage->update();

	const auto nodes =
		m_tspGraph.getNodes();

	QString message =
		QString(
			"Selected edge:\n"
			"%1 - %2\n"
			"Distance: %3 km\n\n"
			"Accumulated cost: %4 km"
		)
		.arg(QString::fromStdString(
			nodes[from]->getName()
		))
		.arg(QString::fromStdString(
			nodes[to]->getName()
		))
		.arg(distance)
		.arg(m_totalDistance);

	if (m_fout.device() != nullptr)
	{
		m_fout
			<< "Selected edge: "
			<< QString::fromStdString(
				nodes[from]->getName()
			)
			<< " - "
			<< QString::fromStdString(
				nodes[to]->getName()
			)
			<< " ("
			<< distance
			<< " km)\n";
	}

	QMessageBox::information(
		this,
		"Minimum Spanning Tree",
		message
	);

	if (m_currentStep <
		static_cast<int>(
			m_minimumSpanningTree.size()
			))
	{
		m_stepTimerMST->start(1200);
	}
	else
	{
		onNextMstStep();
	}
}

void MainWindow::on_showTspCircuitButton_clicked()
{
	if (m_tspGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Traveling Salesman Problem",
			"The graph contains no nodes."
		);

		return;
	}

	if (!m_tspGraph.isConnected())
	{
		QMessageBox::warning(
			this,
			"Traveling Salesman Problem",
			"The graph is not connected."
		);

		return;
	}

	m_stepTimerMST->stop();
	m_stepTimerTSP->stop();

	m_tspGraph.floydWarshall();
	m_tspGraph.buildCompleteGraphFromDistances();

	m_minimumSpanningTree =
		m_tspGraph.kruskal();

	m_travelingSalesmanProblem =
		m_tspGraph.travelingSalesmanProblem(
			m_minimumSpanningTree
		);

	m_currentStep = 0;
	m_totalDistance = 0.0;

	m_drawInitialGraph = false;
	m_drawCompleteGraph = false;
	m_drawMinimumSpanningTree = false;
	m_drawTravellingSalesmanProblem = true;

	if (m_fout.device() == nullptr)
	{
		m_file.setFileName("output.txt");

		if (m_file.open(
			QIODevice::WriteOnly |
			QIODevice::Text))
		{
			m_fout.setDevice(&m_file);
		}
	}

	if (m_fout.device() != nullptr)
	{
		m_fout
			<< "Traveling Salesman Problem\n\n";
	}

	ui->travelingSalesmanPage->update();

	onNextTspStep();
}

void MainWindow::onNextTspStep()
{
	if (m_currentStep + 1 >=
		static_cast<int>(
			m_travelingSalesmanProblem.size()
			))
	{
		m_stepTimerTSP->stop();

		QMessageBox::information(
			this,
			"Traveling Salesman Problem",
			QString(
				"The approximate TSP circuit has been generated successfully.\n\n"
				"Total distance: %1 km"
			).arg(m_totalDistance)
		);

		if (m_fout.device() != nullptr)
		{
			m_fout
				<< "Traveling Salesman Problem completed.\n"
				<< "Total distance: "
				<< m_totalDistance
				<< " km.\n\n";
		}

		return;
	}

	int from =
		m_travelingSalesmanProblem[m_currentStep];

	int to =
		m_travelingSalesmanProblem[m_currentStep + 1];

	double distance =
		m_tspGraph.getDistanceMatrix()[from][to];

	m_totalDistance += distance;

	++m_currentStep;

	ui->travelingSalesmanPage->update();

	const auto nodes =
		m_tspGraph.getNodes();

	QString message =
		QString(
			"Next visited city:\n"
			"%1\n\n"
			"Distance: %2 km\n"
			"Total distance: %3 km"
		)
		.arg(QString::fromStdString(
			nodes[to]->getName()
		))
		.arg(distance)
		.arg(m_totalDistance);

	if (m_fout.device() != nullptr)
	{
		m_fout
			<< "Next visited city: "
			<< QString::fromStdString(
				nodes[to]->getName()
			)
			<< "\nDistance: "
			<< distance
			<< " km\n"
			<< "Total distance: "
			<< m_totalDistance
			<< " km\n\n";
	}

	QMessageBox::information(
		this,
		"Traveling Salesman Problem",
		message
	);

	if (m_currentStep + 1 <
		static_cast<int>(
			m_travelingSalesmanProblem.size()
			))
	{
		m_stepTimerTSP->start(1200);
	}
	else
	{
		onNextTspStep();
	}
}

void MainWindow::on_showConnectedComponentsButton_clicked()
{
	if (m_showStronglyConnectedComponents)
	{
		QMessageBox::warning(
			this,
			"Connected Components",
			"Return to the original graph before computing connected components."
		);

		return;
	}

	if (m_manualGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Connected Components",
			"This graph contains no nodes."
		);

		return;
	}

	if (m_manualGraph.isOriented())
	{
		QMessageBox::warning(
			this,
			"Connected Components",
			"Connected components can only be computed for an undirected graph."
		);

		return;
	}

	m_manualGraph.findConnectedComponents();

	QMessageBox::information(
		this,
		"Connected Components",
		QString("The graph contains %1 connected component(s).")
		.arg(
			static_cast<int>(
				m_manualGraph.getConnectedComponents().size()
				)
		)
	);

	ui->drawingArea->update();
}

void MainWindow::on_resetColorsButton_clicked()
{
	if (m_showStronglyConnectedComponents)
	{
		QMessageBox::warning(
			this,
			"Reset Colors",
			"Return to the original graph before resetting colors."
		);

		return;
	}

	if (m_manualGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Reset Colors",
			"This graph contains no nodes."
		);

		return;
	}

	m_manualGraph.resetNodeColors();

	ui->drawingArea->update();
}

void MainWindow::on_showStronglyConnectedComponentsButton_clicked()
{
	if (m_manualGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Strongly Connected Components",
			"This graph contains no nodes."
		);

		return;
	}

	if (m_showStronglyConnectedComponents)
	{
		QMessageBox::information(
			this,
			"Strongly Connected Components",
			"The strongly connected component graph is already displayed."
		);

		return;
	}

	if (!m_manualGraph.isOriented())
	{
		QMessageBox::warning(
			this,
			"Strongly Connected Components",
			"Strongly connected components require a directed graph."
		);

		return;
	}

	m_manualGraph.findStronglyConnectedComponents();

	m_showStronglyConnectedComponents = true;

	ui->undirectedRadioButton->setEnabled(false);
	ui->directedRadioButton->setEnabled(false);

	QMessageBox::information(
		this,
		"Strongly Connected Components",
		QString("The graph contains %1 strongly connected component(s).")
		.arg(
			static_cast<int>(
				m_manualGraph
				.getStronglyConnectedComponents()
				.size()
				)
		)
	);

	ui->drawingArea->update();
}

void MainWindow::on_showAdjacencyListButton_clicked()
{
	if (m_showStronglyConnectedComponents)
	{
		QMessageBox::warning(
			this,
			"Adjacency List",
			"Return to the original graph before displaying the adjacency list."
		);

		return;
	}

	const Matrix adjacency =
		m_manualGraph.getAdjacencyMatrix();

	if (adjacency.empty())
	{
		QMessageBox::information(
			this,
			"Adjacency List",
			"This graph contains no nodes."
		);

		return;
	}

	QString message =
		"Adjacency list:\n\n";

	for (int i = 0;
		i < static_cast<int>(adjacency.size());
		++i)
	{
		message +=
			QString("Node %1: ").arg(i);

		bool foundNeighbour = false;

		for (int j = 0;
			j < static_cast<int>(adjacency[i].size());
			++j)
		{
			if (adjacency[i][j] != 0)
			{
				message +=
					QString::number(j);

				message += " ";

				foundNeighbour = true;
			}
		}

		if (!foundNeighbour)
			message += "no neighbours";

		message += "\n";
	}

	QMessageBox::information(
		this,
		"Adjacency List",
		message
	);
}

void MainWindow::on_saveGraphImageButton_clicked()
{
	if (m_manualGraph.getNodes().empty())
	{
		QMessageBox::information(
			this,
			"Save Graph Image",
			"This graph contains no nodes."
		);

		return;
	}

	QPixmap pixmap = this->grab();

	if (pixmap.save("graph.png"))
	{
		QMessageBox::information(
			this,
			"Save Graph Image",
			"The graph was saved as graph.png."
		);
	}
	else
	{
		QMessageBox::warning(
			this,
			"Save Graph Image",
			"The graph could not be saved."
		);
	}
}

void MainWindow::on_restoreInitialGraphButton_clicked()
{
	if (!m_showStronglyConnectedComponents)
	{
		QMessageBox::information(
			this,
			"Restore Graph",
			"The original graph is already displayed."
		);

		return;
	}

	m_showStronglyConnectedComponents = false;

	ui->undirectedRadioButton->setEnabled(true);
	ui->directedRadioButton->setEnabled(true);

	m_manualGraph.resetNodeColors();

	ui->drawingArea->update();
}