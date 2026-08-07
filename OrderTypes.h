#pragma once
#include <cstdint>

enum class Side : uint8_t {
	BUY,
	SELL,
};

struct Order {
	uint64_t orderID;
	uint64_t price;
	uint32_t quantity;
	uint64_t timestamp;
	Side side;
};