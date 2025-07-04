#pragma once

#include <cstdint>
#include <cstddef>

class RingBuffer {
public:
        RingBuffer(size_t size);
        ~RingBuffer();

        // Delete copy operations
        RingBuffer(const RingBuffer&) = delete;
        RingBuffer& operator=(const RingBuffer&) = delete;

        // Allow move semantics
        RingBuffer(RingBuffer&& other) noexcept;
        RingBuffer& operator=(RingBuffer&& other) noexcept;

	bool push(uint8_t* data, size_t size);
	bool pop(uint8_t* data, size_t size);
	size_t size() const;
	size_t available() const;
	bool empty() const;
	bool full() const;

private:
	uint8_t*	m_buffer = nullptr;
	size_t          m_head = 0;
	size_t          m_tail = 0;
	size_t		m_capacity = 0;
	bool		m_isFull = false;
};
