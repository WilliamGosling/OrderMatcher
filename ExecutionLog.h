#pragma once
#include <vector>
#include <iostream>
#include <format>
#include "OrderTypes.h"
class ExecutionLog {

private:
	std::vector<Trade> TradesExecuted;

public:
	void insertExecutedTrades(uint64_t buyerID, uint64_t sellerID, uint64_t price, uint64_t quantity);
	void printLog();
};