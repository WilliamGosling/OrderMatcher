#include "OrderBook.h"

void OrderBook::addOrder(const Order& newOrder) {

	if (OrderIDMap.find(newOrder.orderID) != OrderIDMap.end()) {
		std::cout << std::format("Order with ID : {} already exists\n", newOrder.orderID);
		return;
	}


	switch (newOrder.side) {
	case Side::BUY:
		BidBook[newOrder.price].push_back(newOrder);
		OrderIDMap[newOrder.orderID] = std::prev(BidBook[newOrder.price].end());
		break;
	case Side::SELL:
		AskBook[newOrder.price].push_back(newOrder);
		OrderIDMap[newOrder.orderID] = std::prev(AskBook[newOrder.price].end());
		break;
	default:
		std::cout << "Invalid Order\n";
	}
}
void OrderBook::removeOrder(const Price price, const enum Side side) {

	switch (side) {
	case Side::BUY:
		BidBook.at(price).pop_front();
		if (BidBook.at(price).empty()) BidBook.erase(price);
		break;
	case Side::SELL:
		AskBook.at(price).pop_front();
		if (AskBook.at(price).empty()) AskBook.erase(price);
		break;
	default:
		std::cout << "Invalid Order\n";
	}
}

void OrderBook::matchOrder() {

	if (BidBook.empty() || AskBook.empty()) return;

	auto bestBid = BidBook.begin();
	auto bestAsk = AskBook.begin();
	if (bestBid->first >= bestAsk->first) {
		std::cout << std::format("\nMATCHED | Bid Order ID: {} | Ask Order ID: {} \n", bestBid->second.front().orderID, bestAsk->second.front().orderID);
		if (bestBid->second.front().quantity == bestAsk->second.front().quantity) {
			OrderIDMap.erase(bestAsk->second.front().orderID);
			OrderIDMap.erase(bestBid->second.front().orderID);
			removeOrder(bestBid->first, Side::BUY);
			removeOrder(bestAsk->first, Side::SELL);
		}
		else if (bestBid->second.front().quantity > bestAsk->second.front().quantity) {
			bestBid->second.front().quantity -= bestAsk->second.front().quantity;
			OrderIDMap.erase(bestAsk->second.front().orderID);
			removeOrder(bestAsk->first, Side::SELL);
			matchOrder();
		}
		else {
			bestAsk->second.front().quantity -= bestBid->second.front().quantity;
			OrderIDMap.erase(bestBid->second.front().orderID);
			removeOrder(bestBid->first, Side::BUY);
			matchOrder();
		}
		return;
	}
	std::cout << "No matching orders\n";
}

// Prints the full OrderBook
void OrderBook::printBook() {
	for (auto pair : BidBook) {
		std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
	}
	for (auto pair : AskBook) {
		std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: SELL | Timestamp: {} |\n", pair.second.front().orderID, pair.first, pair.second.front().quantity, pair.second.front().timestamp);
	}
}
// Prints requested Side's Book
void OrderBook::printBook(Side side) {

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

std::list<Order>::iterator OrderBook::searchOrderByID(OrderID orderID) {

	std::list<Order>::iterator iterator;

	if (!orderID) {
		std::cout << "Invalid Order ID\n";
		return iterator;
	}

	auto map = OrderIDMap.find(orderID);

	if (map == OrderIDMap.end()) {
		std::cout << "Order not found\n";
		return iterator;
	}

	iterator = map->second;


	std::cout << std::format("Order {} found | Price: {}\n", iterator->orderID, iterator->price);

	return iterator;
}

void OrderBook::cancelOrder(OrderID orderID) {

	std::list<Order>::iterator orderIterator = searchOrderByID(orderID);

	Side orderSide = orderIterator->side;
	Price orderPrice = orderIterator->price;

	if (orderSide == Side::BUY) {
		BidBook.at(orderPrice).erase(orderIterator);
		if (BidBook[orderPrice].empty()) {
			BidBook.erase(orderPrice);
		}
	}
	else {
		AskBook.at(orderPrice).erase(orderIterator);
		if (AskBook[orderPrice].empty()) {
			AskBook.erase(orderPrice);
		}
	}
	OrderIDMap.erase(orderID);
}
