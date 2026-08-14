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

	Order* prev = nullptr;
	Order* next = nullptr;
};

struct PriceLevel {
	Order* head = nullptr;
	Order* tail = nullptr;
	uint32_t count = 0;

	bool empty() const { return head == nullptr; }

	void push_back(Order* order) {
		order->next = nullptr;
		order->prev = tail;
		if (tail) tail->next = order;
		else head = order; // First order in list
		tail = order;
		count++;
	}

	void pop_front() {
		if (!head) return;

		head = head->next;
		if (head) head->prev = nullptr;
		else tail = nullptr; // List is empty
		count--;
	}

	void erase(Order* order) {
		if (order->prev) order->prev->next = order->next;
		else head = order->next;

		if (order->next) order->next->prev = order->prev;
		else tail = order->prev;
		count--;
	}
};