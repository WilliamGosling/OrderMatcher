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
	std::cout << "-------------BidBook-------------\n";
	for (auto pair : BidBook) {
		for (auto orders : pair.second) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", orders.orderID, pair.first, orders.quantity, orders.timestamp);
		}
	}
	std::cout << "---------------------------------\n-------------AskBook-------------\n";
	for (auto pair : AskBook) {
		for (auto orders : pair.second) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", orders.orderID, pair.first, orders.quantity, orders.timestamp);
		}
	}
	std::cout << "---------------------------------\n";
}
// Prints requested Side's Book
void OrderBook::printBook(Side side) {

	if (side == Side::BUY) {
		std::cout << "-------------BidBook-------------\n";
		for (auto pair : BidBook) {
			for (auto orders : pair.second) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", orders.orderID, pair.first, orders.quantity, orders.timestamp);
			}
		}
	}
	else {
		std::cout << "-------------AskBook-------------\n";
		for (auto pair : AskBook) {
			for (auto orders : pair.second) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", orders.orderID, pair.first, orders.quantity, orders.timestamp);
			}
		}
	}
	std::cout << "---------------------------------\n";
}

std::list<Order>::iterator OrderBook::searchOrderByID(OrderID orderID, bool& successFlag) {

	std::list<Order>::iterator iterator;

	if (!orderID) {
		std::cout << "Invalid Order ID\n";
		successFlag = false;
		return iterator;
	}

	auto mapOrderID = OrderIDMap.find(orderID);

	if (mapOrderID == OrderIDMap.end()) {
		std::cout << "Order not found\n";
		successFlag = false;
		return iterator;
	}

	iterator = mapOrderID->second;

	successFlag = true;
	return iterator;
}

void OrderBook::cancelOrder(OrderID orderID) {

	bool successFlag = true;
	std::list<Order>::iterator orderIterator = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::cancelOrder>\n";
		return;
	}

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

void OrderBook::modifyOrder(OrderID orderID, Price price, Quantity quantity, Side side) {

	bool successFlag = true;
	std::list<Order>::iterator orderIterator = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::modifyOrder>\n";
		return;
	}

	Price orderPrice = orderIterator->price;
	Quantity orderQuantity = orderIterator->quantity;
	Timestamp orderTime = orderIterator->timestamp;

	// Time priority not altered if Quantity reduced as does not disadvantage Orders newer than it
	if (orderQuantity > quantity && orderPrice == price) {
		orderIterator->quantity = quantity; // Should reduce quantity of Order
	}
	else {
		Order newOrder;
		newOrder.orderID = orderID;
		newOrder.price = price;
		newOrder.quantity = quantity;
		newOrder.side = side;
		newOrder.timestamp = orderTime;
		cancelOrder(orderID);
		addOrder(newOrder);
	}
}

std::list<Order>::iterator OrderBook::getOrderInformation(OrderID orderID) {
	
	bool successFlag = true;
	std::list<Order>::iterator orderIterator = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::cancelOrder>\n";
		return orderIterator;
	}

	std::cout << std::format("Order {} Found\nPrice: {}\nQuantity: {}\nSide: {}\nTimestamp: {}\n", orderIterator->orderID, orderIterator->price, orderIterator->quantity, (orderIterator->side == Side::BUY) ? "Bid" : "Ask", orderIterator->timestamp);
	std::cout << "------------------\n";
	return orderIterator;
}