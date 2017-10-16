#include "Plotter.h"
#include "PlotterImpl.h"

Plotter::Plotter(QWidget* widget)
{
	m_impl = new PlotterImpl(widget);
}

Plotter::~Plotter()
{
	if (m_impl != NULL) delete m_impl;
}

void Plotter::setTimeBuffer(CircularBuffer<double>* timeBufIn)
{
	m_impl->setTimeBuffer(timeBufIn);
}

void Plotter::addDataBuffer(CircularBuffer<double>* dataBufIn)
{
	m_impl->addDataBuffer(dataBufIn);
}

void Plotter::show()
{

	m_impl->show();
}
