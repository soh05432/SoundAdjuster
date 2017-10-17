#include "PlotterImpl.h"

#include <qapplication.h>
#include <qwt_plot_canvas.h> // QwtPlotCanvas
#include <qwt_plot_curve.h> // QwtPlotCurve

#include "Buffers.h"

QApplication* g_application;

PlotterImpl::PlotterImpl(QWidget* parent) 
	: QwtPlot(parent)
{
	//QwtPlotCanvas* canvas = new QwtPlotCanvas();
	
	setAxisScale(QwtPlot::yLeft, -.25, .25);
	setAxisScale(QwtPlot::xBottom, 0.0, 5.5);

	setCanvasBackground(Qt::white);
	resize(683, 384);

	startTimer(10);
}

PlotterImpl::~PlotterImpl()
{
	if (m_time != NULL) delete m_time;
	if (m_timeInOrder) delete m_timeInOrder;
}

void PlotterImpl::setTimeBuffer(CircularBuffer<double>* timeBufIn)
{
	m_time = timeBufIn;
	m_timeInOrder = new double[timeBufIn->getSize()];
}

const int c_colorBeginIdx = 6; // Beginning index for nice colors in Qt::GlobalColor

void PlotterImpl::addDataBuffer(CircularBuffer<double>* dataBufIn)
{
	m_data.push_back(dataBufIn);

	double* raw = new double[dataBufIn->getSize()];
	m_dataInOrder.push_back(raw);

	QwtPlotCurve* curve = new QwtPlotCurve;
	curve->setPen((Qt::GlobalColor)(m_data.size() + c_colorBeginIdx));
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->attach(this);

	m_dataCurves.push_back(new QwtPlotCurve);
}

void PlotterImpl::startGui()
{
	show();
	g_application->exec();
}

void PlotterImpl::timerEvent(QTimerEvent* e)
{
	const CircularBuffer<double>& timeBuf = *m_time;

	if (!timeBuf.isEmpty())
	{
		if (timeBuf.getLatestVal() > 5.5f)
		{
			setAxisScale(QwtPlot::xBottom, timeBuf.getLatestVal() - 5.5f, timeBuf.getLatestVal());
		}

		timeBuf.getOrderedArray(m_timeInOrder);

		for (int i = 0; i < (int)m_data.size(); i++)
		{
			const CircularBuffer<double>& dataBuf = *m_data[i];

			if (!dataBuf.isEmpty())
			{
				dataBuf.getOrderedArray(m_dataInOrder[i]);
				m_dataCurves[i]->setSamples(m_timeInOrder, m_dataInOrder[i], dataBuf.getSize());
			}
		}

		replot();
	}
}
