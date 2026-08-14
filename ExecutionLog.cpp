#include "ExecutionLog.h"

void ExecutionLog::insertExecutedTrades(uint64_t buyerID, uint64_t sellerID, uint64_t price, uint64_t quantity) {

	Trade newTrade{ buyerID, sellerID, price, quantity };

	TradesExecuted.push_back(newTrade);
}

void ExecutionLog::printLog() {
	for (const auto& trade : TradesExecuted) {
		std::cout << std::format("Trade Executed | Buyer : {} | Seller : {} | Quantity : {} | Price : {}\n", trade.buyerID, trade.sellerID, trade.quantity, trade.price);
	}

}