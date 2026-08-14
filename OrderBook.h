#pragma once
#include <iostream>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <format>
#include <chrono>
#include <atomic>
#include <memory_resource>
#include <vector>
#include <algorithm>
#include "OrderTypes.h"
#include "ExecutionLog.h"

using Price = uint64_t;
using OrderID = uint64_t;
using Timestamp = uint64_t;
using Quantity = uint32_t;

using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;

class OrderBook {
private:
	std::pmr::unsynchronized_pool_resource memoryPool;

	std::pmr::map<Price, PriceLevel, std::greater<Price>> BidBook;
	std::pmr::map<Price, PriceLevel> AskBook;
	std::vector<Order*> OrderIDVector;
	std::pmr::unordered_map<OrderID, Order*> FilledOrderMap;

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	ExecutionLog tradeLog;

public:
	OrderBook();
	void addOrder(Order& newOrder); // Adds a new Order
	void removeOrder(const Price price, const enum Side side); // Removes the Front Order at given Price and Side - Order with the top priority
	void cancelOrder(OrderID orderID); // Cancels the Order at the given ID
	void modifyOrder(OrderID orderID, Price price, Quantity quantity, Side side, Timestamp timestamp);
	void matchOrder(); // Matches orders at top of priority
	void matchOrder(Order& order); // Matches the given Market Order with Orders in opposite Side book until full
	void printBook(); // Prints entire state of OrderBook
	void printBook(Side side); // Prints state of Side's Book
	void marketData(uint16_t numberOfRows);
	Order* getOrderInformation(OrderID orderID);
	Order* searchOrderByID(OrderID orderID, bool& successFlag); // Returns an iterator pointing to the Order at the given ID
	std::chrono::microseconds getOrderTimestamp();
};