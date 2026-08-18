#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>

#include "../Graph/graph.h"
#include "../KDTree/kdtree.h"

#include <string>
#include <limits>

using Matrix = std::vector<std::vector<int>>;

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

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

private:
	void drawGraphContent(QPainter& p);
	void drawMap(QPainter& p);
	void drawArrow(QPainter& painter, QPoint start, QPoint end);

	// Map.
	QPointF mapToWindow(
		double longitude,
		double latitude,
		int width,
		int height,
		double zoom = 1.0
	) const;

	Ui::MainWindow* ui;

	Graph m_manualGraph;
	Graph m_weightedGraph;
	Graph m_labyrinthGraph;
	Graph m_mapGraph;

	Node* m_firstNode = nullptr;
	Node* m_pressedNode = nullptr;

	Node* m_weightedFirstNode = nullptr;
	Node* m_weightedPressedNode = nullptr;

	// Map / KD-Tree.
	int m_selectedNode1 = -1;
	int m_selectedNode2 = -1;

	KDTree* m_tree = nullptr;
	std::vector<KDNode> m_points;

	double m_currentZoom = 1.0;

	double m_minLatitude =
		std::numeric_limits<double>::max();

	double m_maxLatitude =
		std::numeric_limits<double>::lowest();

	double m_minLongitude =
		std::numeric_limits<double>::max();

	double m_maxLongitude =
		std::numeric_limits<double>::lowest();

	QPointF m_lastMousePosition;
	bool m_isDragging = false;

	double m_offsetX = 0.0;
	double m_offsetY = 0.0;

	bool drawGraph = false;
	bool drawLabyrinth = false;
	bool m_showStronglyConnectedComponents = false;

	enum class Page
	{
		Graph,
		WeightedGraph,
		Labyrinth,
		Map
	};

	Page m_currentPage = Page::Graph;

	QString fileName;
	Matrix mat;
};

#endif
