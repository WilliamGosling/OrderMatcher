#include "OrderBook.h"

OrderBook::OrderBook()
	: memoryPool(),
	BidBook(&memoryPool),
	AskBook(&memoryPool),
	OrderIDVector(50000000, nullptr),
	FilledOrderMap(&memoryPool)
{
}

void OrderBook::addOrder(Order& newOrder) {

	if (newOrder.orderID >= OrderIDVector.size()) { return; }

	if (OrderIDVector[newOrder.orderID] != nullptr) { return; }

	void* memory = memoryPool.allocate(sizeof(Order), alignof(Order));
	Order* order = new(memory) Order(newOrder);
	order->prev = nullptr;
	order->next = nullptr;

	switch (newOrder.side) {
	case Side::BUY:
		BidBook[order->price].push_back(order);
		break;
	case Side::SELL:
		AskBook[order->price].push_back(order);
		break;
	default:
		std::cout << "Invalid Order\n";
	}
	OrderIDVector[order->orderID] = order;
}
void OrderBook::removeOrder(const Price price, const enum Side side) {
	Order* targetOrder = nullptr;

	if (side == Side::BUY) {
		auto it = BidBook.find(price);
		if (it != BidBook.end()) {
			targetOrder = it->second.head;
			it->second.pop_front();

			if (it->second.empty()) {
				BidBook.erase(it);
			}
		}
	}
	else {
		auto it = AskBook.find(price);
		
		if (it != AskBook.end()) {
			targetOrder = it->second.head;
			it->second.pop_front();

			if (it->second.empty()) {
				AskBook.erase(it);
			}
		}
	}

	if (targetOrder) {
		targetOrder ->~Order();
		memoryPool.deallocate(targetOrder, sizeof(Order), alignof(Order));
	}
}

void OrderBook::matchOrder() {

	while (!BidBook.empty() && !AskBook.empty()) {

		auto bestBid = BidBook.begin();
		auto bestAsk = AskBook.begin();

		if (bestBid->first < bestAsk->first) {
			break;
		}

		if (bestBid->second.head->quantity == bestAsk->second.head->quantity) {
			tradeLog.insertExecutedTrades(bestBid->second.head->orderID, bestAsk->second.head->orderID, bestBid->second.head->quantity, bestAsk->second.head->price);
			OrderIDVector[bestAsk->second.head->orderID] = nullptr;
			OrderIDVector[bestBid->second.head->orderID] = nullptr;
			removeOrder(bestBid->first, Side::BUY);
			removeOrder(bestAsk->first, Side::SELL);
		}
		else if (bestBid->second.head->quantity > bestAsk->second.head->quantity) {
			tradeLog.insertExecutedTrades(bestBid->second.head->orderID, bestAsk->second.head->orderID, bestAsk->second.head->quantity, bestAsk->second.head->price);
			bestBid->second.head->quantity -= bestAsk->second.head->quantity;
			OrderIDVector[bestAsk->second.head->orderID] = nullptr;
			removeOrder(bestAsk->first, Side::SELL);
		}
		else {
			tradeLog.insertExecutedTrades(bestBid->second.head->orderID, bestAsk->second.head->orderID, bestBid->second.head->quantity, bestAsk->second.head->price);
			bestAsk->second.head->quantity -= bestBid->second.head->quantity;
			OrderIDVector[bestBid->second.head->orderID] = nullptr;
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
		if (order.quantity > bestOrder->second.head->quantity) {
			order.quantity -= bestOrder->second.head->quantity;
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.head->orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.head->orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.head->price, bestOrder->second.head->quantity);
			OrderIDVector[bestOrder->second.head->orderID] = nullptr;
			removeOrder(bestOrder->first, bestOrder->second.head->side);
		}
		else if (order.quantity < bestOrder->second.head->quantity) {
			bestOrder->second.head->quantity -= order.quantity;
			order.quantity -= order.quantity;
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.head->orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.head->orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.head->price, bestOrder->second.head->quantity);
			order.status = Status::FILLED;
			FilledOrderMap[order.orderID] = bestOrder->second.head; // Move Filled Market orders into a Filled Map
		}
		else if (order.quantity == bestOrder->second.head->quantity) {
			uint64_t buyer = (order.side == Side::BUY) ? order.orderID : bestOrder->second.head->orderID;
			uint64_t seller = (order.side == Side::SELL) ? order.orderID : bestOrder->second.head->orderID;
			tradeLog.insertExecutedTrades(buyer, seller, bestOrder->second.head->price, bestOrder->second.head->quantity);
			bestOrder->second.head->quantity = 0;
			order.quantity = 0;
			order.status = Status::FILLED;
			FilledOrderMap[order.orderID] = bestOrder->second.head; // Move Filled Market orders into a Filled Map
			OrderIDVector[bestOrder->second.head->orderID] = nullptr;
			removeOrder(bestOrder->first, bestOrder->second.head->side); // Removes order from book
		}
	}
}

// Prints the full OrderBook
void OrderBook::printBook() {
	std::cout << "-------------BidBook-------------\n";
	for (const auto& pair : BidBook) {
		for (Order* current = pair.second.head; current != nullptr; current = current->next) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", current->orderID, pair.first, current->quantity, current->timestamp);
		}
	}

	std::cout << "---------------------------------\n-------------AskBook-------------\n";
	for (const auto& pair : AskBook) {
		for (Order* current = pair.second.head; current != nullptr; current = current->next) {
			std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: SELL | Timestamp: {} |\n", current->orderID, pair.first, current->quantity, current->timestamp);
		}
	}
	std::cout << "---------------------------------\n";
}
// Prints requested Side's Book
void OrderBook::printBook(Side side) {

	if (side == Side::BUY) {
		std::cout << "-------------BidBook-------------\n";
		for (const auto& pair : BidBook) {
			for (Order* current = pair.second.head; current != nullptr; current = current->next) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: BUY | Timestamp: {} |\n", current->orderID, pair.first, current->quantity, current->timestamp);
			}
		}
	}
	else {
		std::cout << "-------------AskBook-------------\n";
		for (const auto& pair : AskBook) {
			for (Order* current = pair.second.head; current != nullptr; current = current->next) {
				std::cout << std::format("Order {} | Price : {} | Quantity: {} | Side: SELL | Timestamp: {} |\n", current->orderID, pair.first, current->quantity, current->timestamp);
			}
		}
	}
	std::cout << "---------------------------------\n";
}

Order* OrderBook::searchOrderByID(OrderID orderID, bool& successFlag) {
	if (!orderID) {
		std::cout << "Invalid Order ID\n";
		successFlag = false;
		return nullptr;
	}

	if (orderID >= OrderIDVector.size()) {
		return nullptr;
	}

	auto targetOrder = OrderIDVector[orderID];

	if (!targetOrder) {
		std::cout << "Order not found\n";
		successFlag = false;
		return nullptr;
	}

	successFlag = true;
	return targetOrder;
}

void OrderBook::cancelOrder(OrderID orderID) {

	bool successFlag = true;
	Order* targetOrder = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Order not found | <OrderBook::cancelOrder>\n";
		return;
	}

	Price orderPrice = targetOrder->price;

	if (targetOrder->side == Side::BUY) {
		BidBook.at(orderPrice).erase(targetOrder);
		if (BidBook.at(orderPrice).empty()) { BidBook.erase(orderPrice); }
	}
	else {
		AskBook.at(orderPrice).erase(targetOrder);
		if (AskBook.at(orderPrice).empty()) { AskBook.erase(orderPrice); }
	}
	OrderIDVector[orderID] = nullptr;
	targetOrder->~Order();
	memoryPool.deallocate(targetOrder, sizeof(Order), alignof(Order));
}

void OrderBook::modifyOrder(OrderID orderID, Price price, Quantity quantity, Side side, Timestamp timestamp) {

	bool successFlag = true;
	Order* targetOrder = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::modifyOrder>\n";
		return;
	}

	Price orderPrice = targetOrder->price;
	Quantity orderQuantity = targetOrder->quantity;

	// Time priority not altered if Quantity reduced as does not disadvantage Orders newer than it
	if (orderQuantity > quantity && orderPrice == price) {
		targetOrder->quantity = quantity; // Should reduce quantity of Order
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

Order* OrderBook::getOrderInformation(OrderID orderID) {

	bool successFlag = true;
	Order* targetOrder = searchOrderByID(orderID, successFlag);

	if (!successFlag) {
		std::cout << "Iterator not found | <OrderBook::cancelOrder>\n";
		return targetOrder;
	}

	std::cout << std::format("Order {} Found\nPrice: {}\nQuantity: {}\nSide: {}\nTimestamp: {}\n", targetOrder->orderID, targetOrder->price, targetOrder->quantity, (targetOrder->side == Side::BUY) ? "Bid" : "Ask", targetOrder->timestamp);
	std::cout << "------------------\n";
	return targetOrder;
}

std::chrono::microseconds OrderBook::getOrderTimestamp() {
	auto currentTime = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(currentTime - start);
}

void OrderBook::marketData(uint16_t numberOfRows) {

	std::map<Price, PriceLevel>::iterator BidIterator;
	std::map<Price, PriceLevel>::iterator AskIterator;
	uint16_t countBid{ 0 };
	uint16_t countAsk{ 0 };
	uint32_t quantityRequestedBid{ 0 };
	uint32_t quantityRequestedAsk{ 0 };

	// Prevents attempting of printing beyond Book Size
	auto rowsPrintBid = (numberOfRows < BidBook.size()) ? numberOfRows : BidBook.size();
	auto rowsPrintAsk = (numberOfRows < AskBook.size()) ? numberOfRows : AskBook.size();

	for (BidIterator = BidBook.begin(); countBid < rowsPrintBid; BidIterator++) {
		quantityRequestedBid = 0;

		// Manual intrusive list traversal
		for (Order* current = BidIterator->second.head; current != nullptr; current = current->next) {
			quantityRequestedBid += current->quantity;
		}
		std::cout << std::format("| Price : {} | No. Orders : {} | Total Quantity : {} | Total Market Cap : {} | Side : {} |\n", BidIterator->first, BidIterator->second.count, quantityRequestedBid, (BidIterator->first * quantityRequestedBid), "Bid");
		countBid++;
	}

	for (AskIterator = AskBook.begin(); countAsk < rowsPrintAsk; AskIterator++) {
		quantityRequestedAsk = 0;

		for (Order* current = AskIterator->second.head; current != nullptr; current = current->next) {
			quantityRequestedAsk += current->quantity;
		}
		std::cout << std::format("| Price : {} | No. Orders : {} | Total Quantity : {} | Total Market Cap : {} | Side : {} |\n", AskIterator->first, AskIterator->second.count, quantityRequestedAsk, (AskIterator->first * quantityRequestedAsk), "Ask");
		countAsk++;
	}
}
