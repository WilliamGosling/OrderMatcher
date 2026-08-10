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

	book.matchOrder();

	book.printBook(); // Should have no Orders

	book.searchOrderByID(501); // Prints the OrderID and Price if found
}


int main() {

	//addOrderTest();
	matchOrderTest();

	return 0;
}