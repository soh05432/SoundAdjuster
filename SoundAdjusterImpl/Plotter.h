#pragma once

#include <memory>

class PlotterImpl;
class QWidget;
template <typename T> class CircularBuffer;
template <typename T> class MovingAvgFilter;

class Plotter
{
  private:

	// std::shared_ptr<PlotterImpl> m_impl;
	PlotterImpl* m_impl;

  public:

	Plotter(QWidget* widget);
	~Plotter();

	void setTimeBuffer(CircularBuffer<double>* timeBufIn);
	void addDataBuffer(CircularBuffer<double>* dataBufIn);

	void show();
};