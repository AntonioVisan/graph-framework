#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

#include "../Graph/graph.h"

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
	void on_radioButton_toggled(bool checked);
	void on_radioButton_2_toggled(bool checked);

private:
	void drawArrow(QPainter& painter, QPoint start, QPoint end);

	Ui::MainWindow* ui;
	Graph m_graph;

	Node* m_firstNode = nullptr;
	Node* m_pressedNode = nullptr;
};

#endif
