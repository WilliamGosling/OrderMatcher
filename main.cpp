#include "OrderBook.h"

bool addOrderTest() {

	OrderBook book;
	Order newOrder;
	newOrder.orderID = 1234;
	newOrder.price = 50;
	newOrder.quantity = 10;
	newOrder.side = Side::BUY;
	newOrder.timestamp = 1234567;
	const Order order = newOrder;

	Order newOrder2;
	newOrder2.orderID = 1234;
	newOrder2.price = 51;
	newOrder2.quantity = 10;
	newOrder2.side = Side::BUY;
	newOrder2.timestamp = 1234567;
	const Order order2 = newOrder2;


	book.addOrder(order);
	book.addOrder(order2);
	book.printBook(order.side);
	book.removeOrder(51, Side::BUY);

	std::cout << "--------------------\n";

	book.printBook(order.side);

	return true;
}

void matchOrderTest() {

	OrderBook book;
	Order BidOrder;
	Order AskOrder;

	BidOrder.orderID = 500;
	BidOrder.price = 100;
	BidOrder.quantity = 51;
	BidOrder.side = Side::BUY;
	BidOrder.timestamp = 123456;

	AskOrder.orderID = 501;
	AskOrder.price = 100;
	AskOrder.quantity = 52;
	AskOrder.side = Side::SELL;
	AskOrder.timestamp = 1234567;

	book.addOrder(BidOrder);
	book.addOrder(AskOrder);

	book.printBook(); // Should have both Orders

	std::cout << "State after cancelling Order 500\n";
	book.cancelOrder(500);

	//book.matchOrder(); // Should now fail with cancel of order 501

	Order AskOrder2;
	AskOrder2.orderID = 503;
	AskOrder2.price = 100;
	AskOrder2.quantity = 52;
	AskOrder2.side = Side::SELL;
	AskOrder2.timestamp = 1234567;

	book.addOrder(AskOrder2);

	book.printBook(Side::SELL); 

	book.modifyOrder(501, 100, 53, Side::SELL);

	book.printBook(); // Should have 500 Orders

}

int main() {

	matchOrderTest();

	return 0;
}