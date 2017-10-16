#pragma once

#include <cfloat>
#include <Common.h>

typedef unsigned int UINT;

template <typename T>
class CircularBuffer
{
protected:

	T* m_buffer;
	UINT m_size;
	UINT m_beginIndex;
	bool m_filled;

public:

	CircularBuffer( UINT size = 100 ) :
		m_size( size ),
		m_beginIndex( 0 ),
		m_filled( false )
	{
		m_buffer = new T[m_size];
	}
	
	virtual ~CircularBuffer()
	{
		delete[] m_buffer;
	}
	
	virtual void addValue( const T val )
	{
		m_buffer[m_beginIndex] = val;
		
		m_beginIndex = (m_beginIndex + 1) % m_size;
		
		if ( !m_filled && m_beginIndex == 0 )
		{
			m_filled = true;
		}
	}
	
	const T& getLatestVal() const
	{
		ASSERT( !isEmpty(), "Buffer must not be empty" );
		UINT lastIdx = (m_beginIndex != 0) ? m_beginIndex - 1 : m_size - 1;
		return m_buffer[lastIdx];
	}
	
	const T& getEarliestVal() const
	{
		ASSERT( !isEmpty(), "Buffer must not be empty" );
		
		if ( m_filled )
		{
			return m_buffer[(m_beginIndex + 1) % m_size];
		}
		else
		{
			return m_buffer[0];
		}
	}
	
	UINT getFullSize() const
	{
		return m_size;
	}
	
	UINT getSize() const
	{
		if ( m_filled )
		{
			return m_size;
		}
		else
		{
			return m_beginIndex;
		}
	}
	
	bool isEmpty() const
	{
		return (!m_filled && m_beginIndex == 0);
	}

	bool isFilled() const
	{
		return m_filled;
	}

	void getOrderedArray( T* arr ) const
	{
		UINT idxArr = 0;

		if ( m_filled )
		{
			std::memcpy( arr, &m_buffer[m_beginIndex], sizeof( T ) * (m_size - m_beginIndex) );
			std::memcpy( &arr[m_size - m_beginIndex], m_buffer, sizeof( T ) * m_beginIndex );
		}
		else
		{
			std::memcpy( arr, m_buffer, sizeof( T ) * m_size );
		}
	}

	double getMax() const
	{
		ASSERT( !isEmpty(), "Buffer is not filled" );

		double res = std::FLT_MIN;
		UINT currSize = getSize();

		for ( UINT i = 0; i < currSize(); i++ )
		{
			if ( m_buffer[i] > res )
			{
				res = m_buffer[i];
			}
		}

		return res;
	}

	double getMin() const
	{
		ASSERT( !isEmpty(), "Buffer is not filled" );

		double res = std::FLT_MAX;
		UINT currSize = getSize();

		for ( UINT i = 0; i < currSize; i++ )
		{
			if ( m_buffer[i] < res )
			{
				res = m_buffer[i];
			}
		}

		return res;
	}
};

template <typename T>
class MovingAvgFilter : public CircularBuffer<T>
{
	T m_sum;

public:

	MovingAvgFilter( UINT size = 100 ) :
		CircularBuffer( size ),
		m_sum( 0.f )
	{

	}

	virtual ~MovingAvgFilter()
	{

	}

	virtual void addValue( const T val ) override
	{
		CircularBuffer::addValue( val );

		m_sum += val;

		if ( !m_filled && m_beginIndex == 0 )
		{
			m_filled = true;
		}

		if ( m_filled )
		{
			m_sum -= m_buffer[m_beginIndex];
		}
	}

	const T getAverage() const
	{
        if ( getSize() > 0 )
        {
            return m_sum / getSize();
        }
        return 0;
	}
};