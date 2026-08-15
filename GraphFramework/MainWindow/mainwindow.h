#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

#include "../Graph/graph.h"

#include <string>

using matrix = std::vector<std::vector<int>>;

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

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

	// Labyrinth
	void on_openFileButton_clicked();
	void on_findGraphPathsButton_clicked();
	void on_drawLabyrinthButton_clicked();
	void on_findLabyrinthPathsButton_clicked();
	void on_drawGraphButton_clicked();
	void on_clearDrawingButton_clicked();
	void on_clearPathsButton_clicked();

private:
	void drawArrow(QPainter& painter, QPoint start, QPoint end);

	Ui::MainWindow* ui;

	Graph m_manualGraph;
	Graph m_labyrinthGraph;

	Node* m_firstNode = nullptr;
	Node* m_pressedNode = nullptr;

	bool drawGraph = false;
    bool drawLabyrinth = false;

	enum class Page
	{
		Graph,
		Labyrinth
	};

	Page m_currentPage = Page::Graph;

    QString fileName;
    matrix mat;
};

#endif
