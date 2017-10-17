#pragma once

#include <qwt_plot.h>
#include <vector>

class QwtPlotCurve;
template <typename T> class CircularBuffer;
template <typename T> class MovingAvgFilter;

class PlotterImpl : public QwtPlot
{
  private:
	CircularBuffer<double>* m_time;
	std::vector<CircularBuffer<double>*> m_data;
	double* m_timeInOrder;
	std::vector<double*> m_dataInOrder;

	double* m_timeBuffer;
	double* m_sampledAttenuatedBuffer;
	double* m_averageAttenuatedBuffer;
	double* m_optimumAttenuatedBuffer;

	std::vector<QwtPlotCurve*> m_dataCurves;

  public:

	PlotterImpl(QWidget* parent);
	~PlotterImpl();

	void setTimeBuffer(CircularBuffer<double>* timeBufIn);
	void addDataBuffer(CircularBuffer<double>* dataBufIn);

	void startGui();

  protected:

	void timerEvent(QTimerEvent* e);
};
