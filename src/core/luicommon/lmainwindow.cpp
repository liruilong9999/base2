#include "lmainwindow.h"

LMainwindow::LMainwindow(QWidget * parent)
	: QMainWindow(parent),
	_pLWidget(new LWidget(this))
{
    setCentralWidget(_pLWidget);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

LMainwindow::~LMainwindow()
{
	_pLWidget = nullptr;
}

void LMainwindow::show()
{
	QMainWindow::show();
}

void LMainwindow::showLMainWindow()
{
	this->show();
}

LWidget * LMainwindow::lwidget() const
{
	return _pLWidget;
}
