#pragma once
#include <iostream>
#include <cstdint>
#include <map>
#include <list>
#include <format>
#include "OrderTypes.h"

using Price = uint64_t;
using OrderID = uint64_t;
using Timestamp = uint64_t;
using Quantity = uint32_t;

class OrderBook {
private:
	std::map<Price, std::list<Order>> BidBook; // Greater sorts keys in descending order
	std::map<Price, std::list<Order>> AskBook;
	std::map<OrderID, std::list<Order>::iterator> OrderIDMap;
	
public:
	void addOrder(const Order& newOrder); // Adds a new Order
	void removeOrder(const Price price, const enum Side side); // Removes the Front Order at given Price and Side - Order with the top priority
	void cancelOrder(OrderID orderID); // Cancels the Order at the given ID
	void modifyOrder(OrderID orderID, Price price, Quantity quantity, Side side);
	void matchOrder(); // Matches orders at top of priority
	void printBook(); // Prints entire state of OrderBook
	void printBook(Side side); // Prints state of Side's Book
	std::list<Order>::iterator getOrderInformation(OrderID orderID);
	std::list<Order>::iterator searchOrderByID(OrderID orderID, bool& successFlag); // Returns an iterator pointing to the Order at the given ID
};