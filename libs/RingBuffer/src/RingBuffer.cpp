#include "RingBuffer.hpp"

#include <cstring>

RingBuffer::RingBuffer(size_t size)
: m_capacity(size)
{
	m_buffer = new uint8_t[size];
}

RingBuffer::~RingBuffer()
{
        delete[] m_buffer;
}

RingBuffer::RingBuffer(RingBuffer&& other) noexcept
        : m_buffer(other.m_buffer)
        , m_head(other.m_head)
        , m_tail(other.m_tail)
        , m_capacity(other.m_capacity)
        , m_isFull(other.m_isFull)
{
        other.m_buffer = nullptr;
        other.m_head = 0;
        other.m_tail = 0;
        other.m_capacity = 0;
        other.m_isFull = false;
}

RingBuffer& RingBuffer::operator=(RingBuffer&& other) noexcept
{
        if (this != &other)
        {
                delete[] m_buffer;

                m_buffer = other.m_buffer;
                m_head = other.m_head;
                m_tail = other.m_tail;
                m_capacity = other.m_capacity;
                m_isFull = other.m_isFull;

                other.m_buffer = nullptr;
                other.m_head = 0;
                other.m_tail = 0;
                other.m_capacity = 0;
                other.m_isFull = false;
        }

        return *this;
}

bool RingBuffer::push(uint8_t* data, size_t size)
{
	if (! data || ! size || full())
		return false;

	if (size > available())
		return false;

	// If the data does not wrap around the end of the buffer,
	// we can simply copy it into the buffer
	// at the position pointed to by the head.
	if (m_head + size <= m_capacity)
	{
		memcpy(m_buffer + m_head, data, size);
	}
	else
	{
		// If the data does wrap around the end of the buffer,
		// we need to split it into two parts
		// and copy each part into the buffer separately.
		size_t firstPart = m_capacity - m_head;

		// The first part is copied into the space
		memcpy(m_buffer + m_head, data, firstPart);

		// remaining at the end of the buffer, and the second part
		// is copied from the start of the buffer.
		memcpy(m_buffer, data + firstPart, size - firstPart);
	}

	// Move the head forward by the size of the data,
	// wrapping around to the start of the buffer if necessary.
	m_head = (m_head + size) % m_capacity;

	if (m_head == m_tail)
		m_isFull = true;

	// If we've made it to this point, then we have successfully
	// pushed the data into the buffer.
	return true;
}

bool RingBuffer::pop(uint8_t* data, size_t size)
{
	if (! data || ! size || empty())
		return false;

	if (size > this->size())
		return false;

	// If the data does not wrap around the end of the buffer,
	// we can simply copy it from the buffer
	// at the position pointed to by the tail.
	if (m_tail + size <= m_capacity)
	{
		memcpy(data, m_buffer + m_tail, size);
	}
	else
	{
		// If the data does wrap around the end of the buffer,
		// we need to split it into two parts
		// and copy each part from the buffer separately.
		size_t firstPart = m_capacity - m_tail;

		// The first part is copied from the space
		memcpy(data, m_buffer + m_tail, firstPart);

		// remaining at the end of the buffer, and the second part is
		// copied from the start of the buffer.
		memcpy(data + firstPart, m_buffer, size - firstPart);
	}

	// Move the tail forward by the size of the data, wrapping around to the start of the buffer if necessary.
	m_tail = (m_tail + size) % m_capacity;
	m_isFull = false;

	// If we've made it to this point, then we have successfully popped the data from the buffer.
	return true;
}

size_t RingBuffer::size() const
{
	if (m_head == m_tail)
	{
		return m_isFull ? m_capacity : 0;
	}
	else if (m_head > m_tail)
	{
		return m_head - m_tail;
	}

	// m_tail > m_head
	return m_capacity - m_tail + m_head;
}

size_t RingBuffer::available() const
{
        // The number of free bytes is simply the capacity minus the
        // number of bytes currently stored in the buffer.
        return m_capacity - size();
}

bool RingBuffer::empty() const
{
	return (! full() && (m_head == m_tail));
}

bool RingBuffer::full() const
{
	return m_isFull;
}
