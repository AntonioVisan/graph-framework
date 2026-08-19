#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTextStream>
#include <QTimer>
#include <QWheelEvent>

#include <limits>
#include <string>
#include <vector>

#include "../Graph/graph.h"
#include "../KDTree/kdtree.h"
#include "../FlowNetwork/flownetwork.h"
#include "../VirtualEdge/virtualedge.h"
#include "../Node/node.h"

using Matrix = std::vector<std::vector<int>>;

QT_BEGIN_NAMESPACE
namespace Ui 
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

private slots:
	//Graph
	void on_undirectedRadioButton_toggled(bool checked);
	void on_directedRadioButton_toggled(bool checked);

	// Theme 4 - Connected Components
	void on_showConnectedComponentsButton_clicked();
	void on_resetColorsButton_clicked();

	// Theme 4 - Strongly Connected Components
	void on_showStronglyConnectedComponentsButton_clicked();

	// Theme 4 - Graph utilities
	void on_showAdjacencyListButton_clicked();
	void on_saveGraphImageButton_clicked();
	void on_restoreInitialGraphButton_clicked();

	// Weighted Graph
	void on_addEdgeWeightButton_clicked();
	void on_showEdgeWeightsButton_clicked();
	void on_findShortestPathsButton_clicked();
	void on_topologicalSortButton_clicked();

	// Labyrinth
	void on_openFileButton_clicked();
	void on_findGraphPathsButton_clicked();
	void on_drawLabyrinthButton_clicked();
	void on_findLabyrinthPathsButton_clicked();
	void on_drawGraphButton_clicked();
	void on_clearDrawingButton_clicked();
	void on_clearPathsButton_clicked();

	// Theme 6 - Traveling Salesman Problem
	void on_showInitialGraphButton_clicked();
	void on_showCompleteGraphButton_clicked();
	void on_showMinimumSpanningTreeButton_clicked();
	void on_showTspCircuitButton_clicked();

	// Theme 7 - Maximum Flow
	void on_runMaximumFlowButton_clicked();
	void on_resetNetworkButton_clicked();
	void on_selectSourceButton_clicked();
	void on_selectTargetButton_clicked();

private:
	// Drawing
	void drawGraphContent(QPainter& p);
	void drawMap(QPainter& p);
	
	void drawArrow(
		QPainter& painter,
		QPoint start,
		QPoint end
	);

	// Theme 7 - Flow Network drawing
	void drawArrow(
		QPainter& painter,
		QPoint start,
		QPoint end,
		double nodeRadius
	);

	void drawMinCut(Node* source);

	// Map.
	QPointF mapToWindow(
		double longitude,
		double latitude,
		int width,
		int height,
		double zoom = 1.0
	) const;

	// Traveling Salesman Problem.
	QPointF tspMapToWindow(
		double longitude,
		double latitude,
		int width,
		int height,
		double zoom = 1.0
	) const;

	// Theme 6
	void computeBoundingBox();

	void showInitialGraphDistances();
	void showCompleteGraphDistances();

	void drawNodes(QPainter& painter);
	void drawInitialGraph(QPainter& painter);
	void drawCompleteGraph(QPainter& painter);
	void drawMinimumSpanningTree(QPainter& painter);
	void drawTravelingSalesmanProblem(QPainter& painter);

	void onNextMstStep();
	void onNextTspStep();

	// Theme 7 - Maximum Flow
	void AddNode(int index, QPoint point);
	void ResetNodeColors();
	void ResetEdgeColors();

private:
	Ui::MainWindow* ui;

	// Graphs
	Graph m_manualGraph;
	Graph m_weightedGraph;
	Graph m_labyrinthGraph;
	Graph m_mapGraph;
	Graph m_tspGraph;

	Node* m_firstNode = nullptr;
	Node* m_pressedNode = nullptr;

	Node* m_weightedFirstNode = nullptr;
	Node* m_weightedPressedNode = nullptr;

	// Map / KD-Tree.
	int m_selectedNode1 = -1;
	int m_selectedNode2 = -1;

	KDTree* m_tree = nullptr;
	std::vector<KDNode> m_points;

	double m_mapZoom = 1.0;
	double m_tspZoom = 1.0;

	double m_minLatitude =
		std::numeric_limits<double>::max();

	double m_maxLatitude =
		std::numeric_limits<double>::lowest();

	double m_minLongitude =
		std::numeric_limits<double>::max();

	double m_maxLongitude =
		std::numeric_limits<double>::lowest();

	double m_tspMinLatitude =
		std::numeric_limits<double>::max();

	double m_tspMaxLatitude =
		std::numeric_limits<double>::lowest();

	double m_tspMinLongitude =
		std::numeric_limits<double>::max();

	double m_tspMaxLongitude =
		std::numeric_limits<double>::lowest();

	QPointF m_lastMousePosition;
	bool m_isDragging = false;

	double m_offsetX = 0.0;
	double m_offsetY = 0.0;

	// General drawing state
	bool drawGraph = false;
	bool drawLabyrinth = false;
	bool m_showStronglyConnectedComponents = false;

	// Theme 6 drawing state
	bool m_drawInitialGraph = false;
	bool m_drawCompleteGraph = false;
	bool m_drawMinimumSpanningTree = false;
	bool m_drawTravellingSalesmanProblem = false;

	// Theme 6 - MST
	QTimer* m_stepTimerMST = nullptr;
	std::vector<Edge> m_minimumSpanningTree;

	// Theme 6 - TSP
	QTimer* m_stepTimerTSP = nullptr;
	int m_currentStep = 0;
	double m_totalDistance = 0.0;
	std::vector<int> m_travelingSalesmanProblem;

	// Theme 7 - Maximum Flow
	FlowNetwork m_flowNetwork{ 0 };
	std::vector<Node*> m_nodes;
	std::vector<VirtualEdge*> m_edges;

	Node* m_source = nullptr;
	Node* m_target = nullptr;

	bool m_algorithmFinished = false;
	bool m_selectSource = false;
	bool m_selectTarget = false;

	enum class Page
	{
		Graph,
		WeightedGraph,
		Labyrinth,
		Map,
		TravelingSalesman,
		MaximumFlow
	};

	Page m_currentPage = Page::Graph;

	QString fileName;
	Matrix mat;

	// Theme 6 file output
	QFile m_file;
	QTextStream m_fout;
};

#endif
