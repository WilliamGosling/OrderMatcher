#pragma once
#include <cstdint>

enum class Side : uint8_t {
	BUY,
	SELL,
};

enum class OrderType : uint8_t {
	LIMIT,
	MARKET,
};

enum class Status : uint8_t {
	NOT_FILLED,
	FILLED,
};

struct Trade {
	uint64_t buyerID;
	uint64_t sellerID;
	uint64_t price;
	uint64_t quantity;
};

struct Order {
	uint64_t orderID;
	uint64_t price;
	uint32_t quantity;
	uint64_t timestamp;
	Side side;
	OrderType type;
	Status status;
};