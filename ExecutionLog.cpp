#include "ExecutionLog.h"

void ExecutionLog::insertExecutedTrades(uint64_t buyerID, uint64_t sellerID, uint64_t price, uint64_t quantity) {

	TradesExecuted.emplace_back(buyerID, sellerID, price, quantity);
}

void ExecutionLog::printLog() {
	for (const auto& trade : TradesExecuted) {
		std::cout << std::format("Trade Executed | Buyer : {} | Seller : {} | Quantity : {} | Price : {}\n", trade.buyerID, trade.sellerID, trade.quantity, trade.price);
	}

}