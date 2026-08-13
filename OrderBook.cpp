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
	auto addedTime = static_cast<uint64_t>(getOrderTimestamp().count());

	std::cout << std::format(" Order {} Added @ {}\n", newOrder.orderID, addedTime);
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

	while (!BidBook.empty() && !AskBook.empty()) {

		auto bestBid = BidBook.begin();
		auto bestAsk = AskBook.begin();

		if (bestBid->first < bestAsk->first) {
			break;
		}

		std::cout << std::format("\nMATCHED | Bid Order ID: {} | Ask Order ID: {} \n", bestBid->second.front().orderID, bestAsk->second.front().orderID);
		if (bestBid->second.front().quantity == bestAsk->second.front().quantity) {
			tradeLog.insertExecutedTrades(bestBid->second.front().orderID, bestAsk->second.front().orderID, bestBid->second.front().quantity, bestAsk->second.front().price);
			OrderIDMap.erase(bestAsk->second.front().orderID);
			OrderIDMap.erase(bestBid->second.front().orderID);
			removeOrder(bestBid->first, Side::BUY);
			removeOrder(bestAsk->first, Side::SELL);
		}
		else if (bestBid->second.front().quantity > bestAsk->second.front().quantity) {
			tradeLog.insertExecutedTrades(bestBid->second.front().orderID, bestAsk->second.front().orderID, bestAsk->second.front().quantity, bestAsk->second.front().price);
			bestBid->second.front().quantity -= bestAsk->second.front().quantity;
			OrderIDMap.erase(bestAsk->second.front().orderID);
			removeOrder(bestAsk->first, Side::SELL);
		}
		else {
			tradeLog.insertExecutedTrades(bestBid->second.front().orderID, bestAsk->second.front().orderID, bestBid->second.front().quantity, bestAsk->second.front().price);
			bestAsk->second.front().quantity -= bestBid->second.front().quantity;
			OrderIDMap.erase(bestBid->second.front().orderID);
			removeOrder(bestBid->first, Side::BUY);

		}
	}

	std::cout << "No matching orders\n";
}

void OrderBook::matchOrder(Order& order) {

	if (order.type != OrderType::MARKET) {
		std::cout << "OrderType is not of type : MARKET\n";
		return;
	}

	while (order.quantity > 0 && !AskBook.empty() && !BidBook.empty()) {
		auto bestOrder = (order.side == Side::BUY) ? AskBook.begin() : BidBook.begin();
		if (order.quantity > bestOrder->second.front().quantity) {
			order.quantity -= bestOrder->second.front().quantity;
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.front().orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.front().orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.front().price, bestOrder->second.front().quantity);
			OrderIDMap.erase(bestOrder->second.front().orderID);
			removeOrder(bestOrder->first, bestOrder->second.front().side);
		}
		else if (order.quantity < bestOrder->second.front().quantity) {
			bestOrder->second.front().quantity -= order.quantity;
			order.quantity -= order.quantity;
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.front().orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.front().orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.front().price, bestOrder->second.front().quantity);
			order.status = Status::FILLED;
			FilledOrderMap[order.orderID] = bestOrder->second.front(); // Move Filled Market orders into a Filled Map
		}
		else if (order.quantity == bestOrder->second.front().quantity) {
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.front().orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.front().orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.front().price, bestOrder->second.front().quantity);
			bestOrder->second.front().quantity = 0;
			order.quantity = 0;
			order.status = Status::FILLED;
			FilledOrderMap[order.orderID] = bestOrder->second.front(); // Move Filled Market orders into a Filled Map
			OrderIDMap.erase(bestOrder->second.front().orderID); // Removes opposite order from ID Iterator Map
			removeOrder(bestOrder->first, bestOrder->second.front().side); // Removes order from book
		}
	}
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
		if (BidBook.at(orderPrice).empty()) {
			BidBook.erase(orderPrice);
		}
	}
	else {
		AskBook.at(orderPrice).erase(orderIterator);
		if (AskBook.at(orderPrice).empty()) {
			AskBook.erase(orderPrice);
		}
	}
	OrderIDMap.erase(orderID);
	auto cancelledTime = static_cast<uint64_t>(getOrderTimestamp().count());

	std::cout << std::format(" Order {} Cancelled @ {}\n", orderID, cancelledTime);
}

void OrderBook::modifyOrder(OrderID orderID, Price price, Quantity quantity, Side side, Timestamp timestamp) {

	bool successFlag = true;
	std::list<Order>::iterator orderIterator = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::modifyOrder>\n";
		return;
	}

	Price orderPrice = orderIterator->price;
	Quantity orderQuantity = orderIterator->quantity;

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
		newOrder.timestamp = timestamp;
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

std::chrono::microseconds OrderBook::getOrderTimestamp() {
	auto currentTime = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(currentTime - start);
}

void OrderBook::marketData(uint16_t numberOfRows) {

	std::map<Price, std::list<Order>>::iterator BidIterator;
	std::map<Price, std::list<Order>>::iterator AskIterator;
	std::list<Order>::iterator OrderIterator;
	uint16_t countBid{ 0 };
	uint16_t countAsk{ 0 };
	uint32_t orderCountBid{ 0 };
	uint32_t orderCountAsk{ 0 };
	uint32_t quantityRequestedBid{ 0 };
	uint32_t quantityRequestedAsk{ 0 };

	// Prevents attempting of printing beyond Book Size
	auto rowsPrintBid = (numberOfRows < BidBook.size()) ? numberOfRows : BidBook.size();
	auto rowsPrintAsk = (numberOfRows < AskBook.size()) ? numberOfRows : AskBook.size();

	for (BidIterator = BidBook.begin(); countBid < rowsPrintBid; BidIterator++) {
		quantityRequestedBid = 0;
		orderCountBid = 0;
		for (OrderIterator = BidIterator->second.begin(); orderCountBid < BidIterator->second.size(); OrderIterator++) {
			quantityRequestedBid += OrderIterator->quantity;
			orderCountBid++;
		}
		std::cout << std::format("| Price : {} | No. Orders : {} | Total Quantity : {} | Total Market Cap : {} | Side : {} |\n", BidIterator->first, BidIterator->second.size(), quantityRequestedBid, (BidIterator->first * quantityRequestedBid), "Bid");
		countBid++;
	}

	for (AskIterator = AskBook.begin(); countAsk < rowsPrintAsk; AskIterator++) {
		quantityRequestedAsk = 0;
		orderCountAsk = 0;
		for (OrderIterator = AskIterator->second.begin(); orderCountAsk < AskIterator->second.size(); OrderIterator++) {
			quantityRequestedAsk += OrderIterator->quantity;
			orderCountAsk++;
		}
		std::cout << std::format("| Price : {} | No. Orders : {} | Total Quantity : {} | Total Market Cap : {} | Side : {} |\n", AskIterator->first, AskIterator->second.size(), quantityRequestedAsk, (AskIterator->first * quantityRequestedAsk), "Ask");
		countAsk++;
	}
}
