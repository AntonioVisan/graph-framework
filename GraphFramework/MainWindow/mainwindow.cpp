#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, drawGraph(false)
	, drawLabyrinth(false)
{
	ui->setupUi(this);

	m_weightedGraph.changeState();

	// Navigation between application pages.
	connect(ui->actionGraph, &QAction::triggered, this, [this]()
		{
			m_currentPage = Page::Graph;

			ui->stackedWidget->setCurrentWidget(ui->graphPage);

			update();
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

	m_currentPage = Page::Graph;
	ui->stackedWidget->setCurrentWidget(ui->graphPage);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* m)
{
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

	const int offsetX = 100;
	const int offsetY = 50;

	int mouseX = m->pos().x() - offsetX;
	int mouseY = m->pos().y() - offsetY;

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
			update();
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

	// Draw the manually created graph on the Graph page.
	if (m_currentPage == Page::Graph)
	{
		const int graphOffsetX = 100;
		const int graphOffsetY = 50;

		std::vector<Node*> nodes = m_manualGraph.getNodes();

		for (const auto* n : nodes)
		{
			QRect r(
				n->getX() + graphOffsetX - 10,
				n->getY() + graphOffsetY - 10,
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

		std::vector<Edge> edges = m_manualGraph.getEdges();

		for (const auto& ed : edges)
		{
			QPoint firstNode(
				ed.getFirst()->getX() + graphOffsetX,
				ed.getFirst()->getY() + graphOffsetY
			);

			QPoint secondNode(
				ed.getSecond()->getX() + graphOffsetX,
				ed.getSecond()->getY() + graphOffsetY
			);

			p.drawLine(firstNode, secondNode);

			if (m_manualGraph.isOriented())
				drawArrow(p, firstNode, secondNode);
		}
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

	if (m_pressedNode)
	{
		const int offsetX = 100;
		const int offsetY = 50;
		
		const int margin = 10;
		const int nodeRadius = 10;

		// Mouse position in MainWindow coordinates.
		int mouseX = m->pos().x();
		int mouseY = m->pos().y();

		// Keep the whole node inside the window.
		if (mouseX < nodeRadius + margin)
			mouseX = nodeRadius + margin;

		if (mouseX > width() - nodeRadius - margin)
			mouseX = width() - nodeRadius - margin;

		// Keep the whole node below the menu bar.
		int topLimit = menuBar()->geometry().bottom() + margin + 2 * nodeRadius + 10;

		if (mouseY < topLimit)
			mouseY = topLimit;

		if (mouseY > height() - nodeRadius - margin)
			mouseY = height() - nodeRadius - margin;

		// Convert back to graph coordinates.
		int newX = mouseX - offsetX;
		int newY = mouseY - offsetY;

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
			update();
		}
	}
}

void MainWindow::mousePressEvent(QMouseEvent* m)
{
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

	const int offsetX = 100;
	const int offsetY = 50;

	int mouseX = m->pos().x() - offsetX;
	int mouseY = m->pos().y() - offsetY;

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

// Switch between oriented and non-oriented graph modes.
void MainWindow::on_undirectedRadioButton_toggled(bool checked)
{
	if (checked)
	{
		m_manualGraph.changeState();
		update();
	}
}

void MainWindow::on_directedRadioButton_toggled(bool checked)
{
	if (checked)
	{
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
