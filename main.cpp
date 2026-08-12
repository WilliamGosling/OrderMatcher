#include "OrderBook.h"

void matchOrderTest() {

	OrderBook book;
	Order BidOrder;
	Order BidOrder2;
	Order AskOrder;
	Order AskOrder2;

	BidOrder.orderID = 500;
	BidOrder.price = 100;
	BidOrder.quantity = 51;
	BidOrder.side = Side::BUY;
	BidOrder.timestamp = static_cast<uint64_t>(book.getOrderTimestamp().count());

	BidOrder2.orderID = 505;
	BidOrder2.price = 103;
	BidOrder2.quantity = 51;
	BidOrder2.side = Side::BUY;
	BidOrder2.timestamp = static_cast<uint64_t>(book.getOrderTimestamp().count());

	AskOrder.orderID = 501;
	AskOrder.price = 101;
	AskOrder.quantity = 52;
	AskOrder.side = Side::SELL;
	AskOrder.timestamp = static_cast<uint64_t>(book.getOrderTimestamp().count());

	AskOrder2.orderID = 502;
	AskOrder2.price = 101;
	AskOrder2.quantity = 52;
	AskOrder2.side = Side::SELL;
	AskOrder2.timestamp = static_cast<uint64_t>(book.getOrderTimestamp().count());

	book.addOrder(BidOrder);
	book.addOrder(BidOrder2);
	book.addOrder(AskOrder);
	book.addOrder(AskOrder2);

	book.printBook();

	//book.printBook(); // Should have both Orders
	//std::cout << "State after cancelling Order 500\n";
	//book.printBook(Side::SELL); 

	//book.modifyOrder(501, 100, 53, Side::SELL, static_cast<uint64_t>(book.getOrderTimestamp().count()));

	//book.printBook(); // Should have 500 Orders
	//auto it = book.getOrderInformation(501);
	//std::cout << std::format("Recieved information : {} {} {} {}\n", it->orderID, it->price, it->quantity, (it->side == Side::BUY) ? "Bid" : "Ask");

	book.marketData(1);

	book.matchOrder();

	book.printBook();


	
}

int main() {

	matchOrderTest();

	return 0;
}