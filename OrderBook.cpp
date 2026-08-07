#include <iostream>
#include <map>
#include <queue>
#include <format>
#include "OrderTypes.h"

using Price = uint64_t;

class OrderBook {
private:
	std::map<Price, std::queue<Order>, std::greater<Price>> BidBook; // Greater sorts keys in descending order
	std::map<Price, std::queue<Order>> AskBook;



public:

	void addOrder(const Order& newOrder) {

		switch (newOrder.side) {
		case Side::BUY:
			BidBook[newOrder.price].push(newOrder);
			break;
		case Side::SELL:
			AskBook[newOrder.price].push(newOrder);
			break;
		default:
			std::cout << "Invalid Order\n";
		}
	}
	void removeOrder(const Price price, const enum Side side) {

		switch (side) {
		case Side::BUY:
			BidBook.at(price).pop();
			if (BidBook.at(price).empty()) BidBook.erase(price);
			break;
		case Side::SELL:
			AskBook.at(price).pop();
			if (AskBook.at(price).empty()) AskBook.erase(price);
			break;
		default:
			std::cout << "Invalid Order\n";
		}
	}

	void matchOrder() {

		if (BidBook.empty() || AskBook.empty()) return;

		auto bestBid = BidBook.begin();
		auto bestAsk = AskBook.begin();
		if (bestBid->first >= bestAsk->first) {
			std::cout << std::format("\nMATCHED | Bid Order ID: {} | Ask Order ID: {} \n", bestBid->second.front().orderID, bestAsk->second.front().orderID);
			removeOrder(bestBid->first, Side::BUY);
			removeOrder(bestAsk->first, Side::SELL);
			return;
		}
		std::cout << "No matching orders\n";
	}
	void printBook() {
		for (auto pair : BidBook) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
		}
		for (auto pair : AskBook) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: SELL | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
		}
	}

	void printBook(Side side) {

		if (side == Side::BUY) {
			for (auto pair : BidBook) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
			}
		}
		else {
			for (auto pair : AskBook) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: SELL | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
			}
		}
	}
};