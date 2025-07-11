#include <gtest/gtest.h>
#include <type_traits>
#include "RingBuffer.hpp"

class RingBufferTest
:	public ::testing::Test
{
protected:
	void SetUp() override
	{
		ringBuffer = new RingBuffer(1024);
	}

	void TearDown() override
	{
		delete ringBuffer;
	}

	RingBuffer* ringBuffer = nullptr;
};

TEST_F (RingBufferTest, Create)
{
	ASSERT_NE(ringBuffer, nullptr); 
}

TEST_F (RingBufferTest, Empty_True)
{
	EXPECT_EQ(ringBuffer->empty(), true);
}

TEST_F (RingBufferTest, Empty_False)
{
	uint8_t data[4] = { 1, 2, 3, 4 };

	ringBuffer->push(data, sizeof(data));

	EXPECT_EQ(ringBuffer->empty(), false);
}

TEST_F (RingBufferTest, Full_True)
{
	uint8_t data[1024] = { 1, 2, 3, 4 };

	ringBuffer->push(data, sizeof(data));

	EXPECT_EQ(ringBuffer->full(), true);
}

TEST_F (RingBufferTest, Full_False)
{
	uint8_t data[4] = { 1, 2, 3, 4 };

	ringBuffer->push(data, sizeof(data));

	EXPECT_EQ(ringBuffer->full(), false);
}

TEST_F (RingBufferTest, Available_NonZero)
{
	uint8_t data[4] = { 1, 2, 3, 4 };

	ringBuffer->push(data, sizeof(data));

	EXPECT_EQ(ringBuffer->available(), 1024 - sizeof(data));
}

TEST_F (RingBufferTest, Available_Zero)
{
	uint8_t data[1024] = { 1, 2, 3, 4 };

	ringBuffer->push(data, sizeof(data));

	EXPECT_EQ(ringBuffer->available(), 0);
}

TEST_F (RingBufferTest, PushAndPop_SameData)
{
    uint8_t data[4] = { 1, 2, 3, 4 };
    uint8_t poppedData[4] = { 0 };

    ringBuffer->push(data, sizeof(data));
    ringBuffer->pop(poppedData, sizeof(poppedData));

    EXPECT_EQ(memcmp(data, poppedData, sizeof(data)), 0);
}

TEST_F (RingBufferTest, BoundaryConditions)
{
    uint8_t data[1024];
    uint8_t poppedData[1024] = { 0 };
    for (int i = 0; i < 1024; i++) data[i] = i % 256;

    EXPECT_TRUE(ringBuffer->push(data, sizeof(data)));
    EXPECT_EQ(ringBuffer->full(), true);

    EXPECT_TRUE(ringBuffer->pop(poppedData, sizeof(poppedData)));
    EXPECT_EQ(ringBuffer->empty(), true);
    EXPECT_EQ(memcmp(data, poppedData, sizeof(data)), 0);
}

TEST_F (RingBufferTest, Overcapacity)
{
    uint8_t data[1025];
    for (int i = 0; i < 1025; i++) data[i] = i % 256;

    EXPECT_FALSE(ringBuffer->push(data, sizeof(data)));
}

TEST_F (RingBufferTest, Underflow)
{
    uint8_t poppedData[1];

    EXPECT_FALSE(ringBuffer->pop(poppedData, sizeof(poppedData)));
}

TEST_F (RingBufferTest, BufferWrapAround)
{
    uint8_t data[512], secondData[512], poppedData[512];
    for (int i = 0; i < 512; i++) 
    {
        data[i] = i % 256;
        secondData[i] = (i + 128) % 256;
    }

    EXPECT_TRUE(ringBuffer->push(data, sizeof(data)));
    EXPECT_TRUE(ringBuffer->pop(poppedData, sizeof(poppedData)));
    EXPECT_EQ(memcmp(data, poppedData, sizeof(data)), 0);

    EXPECT_TRUE(ringBuffer->push(secondData, sizeof(secondData)));
    EXPECT_TRUE(ringBuffer->pop(poppedData, sizeof(poppedData)));
    EXPECT_EQ(memcmp(secondData, poppedData, sizeof(secondData)), 0);
}

TEST_F (RingBufferTest, Available_TailGreaterThanHead)
{
    // Perform a sequence of push/pop operations that will cause the
    // internal tail index to advance past the head index.
    uint8_t firstData[900] = { 0 };
    uint8_t secondData[700] = { 0 };
    uint8_t temp[800];

    ASSERT_TRUE(ringBuffer->push(firstData, sizeof(firstData)));
    ASSERT_TRUE(ringBuffer->pop(temp, 800));
    ASSERT_TRUE(ringBuffer->push(secondData, sizeof(secondData)));

    // After the above operations the buffer contains 800 bytes of data
    // and the tail index is greater than the head index. Available should
    // therefore report the remaining capacity correctly.
    EXPECT_EQ(ringBuffer->size(), 800u);
    EXPECT_EQ(ringBuffer->available(), 1024u - 800u);
}

TEST(RingBufferTypeTraits, CopyAndMoveProperties)
{
    EXPECT_FALSE(std::is_copy_constructible_v<RingBuffer>);
    EXPECT_FALSE(std::is_copy_assignable_v<RingBuffer>);
    EXPECT_TRUE(std::is_move_constructible_v<RingBuffer>);
    EXPECT_TRUE(std::is_move_assignable_v<RingBuffer>);
}

TEST(RingBufferMoveTest, MoveConstructorAndAssignment)
{
    RingBuffer rb1(16);
    uint8_t data[4] = {1, 2, 3, 4};
    ASSERT_TRUE(rb1.push(data, sizeof(data)));

    RingBuffer rb2(std::move(rb1));
    EXPECT_EQ(rb2.size(), sizeof(data));
    EXPECT_EQ(rb1.size(), 0u);

    RingBuffer rb3(8);
    rb3 = std::move(rb2);
    EXPECT_EQ(rb3.size(), sizeof(data));
    EXPECT_EQ(rb2.size(), 0u);
}
