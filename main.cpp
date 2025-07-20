#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "price_data.h"

std::vector<PriceData> loadPriceData(const std::string& filename) {
	std::vector<PriceData> data;
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "Ошибка при открытии файла: " << filename << std::endl;
		return data;
	}

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string dataStr, priceStr;
		if (std::getline(iss, dataStr, ',') && std::getline(iss, priceStr)) {
			PriceData pd;
			pd.data = dataStr;
			pd.price = std::stod(priceStr);
			data.push_back(pd);
		}
	}

	file.close();
	return data;
}

std::vector<double> calculateSMA(const std::vector<PriceData>& prices, int period) {
	std::vector<double> sma;
	double sum = 0.0;

	for (size_t i = 0; i < prices.size(); ++i) {
		sum += prices[i].price;

		if (i >= period) {
			sum -= prices[i - period].price;
		}

		if (i + 1 >= period) {
			sma.push_back(sum / period);
		}
		else {
			sma.push_back(-1.0);
		}
	}

	return sma;
}

int main() {

	std::setlocale(LC_ALL, "Russian");

	std::string filename = "data.csv";
	std::vector<PriceData> prices = loadPriceData(filename);
	
	std::cout << "Загружено " << prices.size() << " записей:" << std::endl;
	for (const auto& p : prices) {
		std::cout << p.data << " -$" << p.price << std::endl;
	}

	int period = 5;
	std::vector<double> smaValues = calculateSMA(prices, period);

	std::cout << "\nДата\t\tЦена\tSMA(" << period << ")\n";
	for (size_t i = 0; i < prices.size(); ++i) {
		std::cout << prices[i].data << "\t$" << prices[i].price << "\t";
		if (i + 1 < period)
			std::cout << "-";
		else
			std::cout << smaValues[i];
		std::cout << std::endl << std::endl;
	}

	int smaPeriod = 5;
	std::vector<double> sma = calculateSMA(prices, smaPeriod);

	std::cout << "\nДата\t\tЦена\tSMA(" << smaPeriod << ")" << std::endl;
	for (size_t i = 0; i < prices.size(); ++i) {
		std::cout << prices[i].data << "\t$" << prices[i].price << "\t";
		if (sma[i] >= 0)
			std::cout << " " << sma[i];
		else
			std::cout << " -";

		std::cout << std::endl;
	}

	for (size_t i = 1; i < prices.size(); ++i) {
		if (sma[i - 1] < 0 || sma[i] < 0) continue;

		double priceYesterday = prices[i - 1].price;
		double priceToday = prices[i].price;

		double smaYesterday = sma[i - 1];
		double smaToday = sma[i];

		if (priceYesterday < smaYesterday && priceToday > smaToday) {
			std::cout << prices[i].data << " - Покупка по цене $" << priceToday << std::endl;
		}
		else if (priceYesterday > smaYesterday && priceToday < smaToday) {
			std::cout << prices[i].data << " - Продажа по цене $" << priceToday << std::endl;
		}
	}

	bool inPosition = false;
	double buyPrice = 0.0;
	double profit = 0.0;

	for (size_t i = 1; i < prices.size(); ++i) {
		if (sma[i - 1] < 0 || sma[i] < 0) continue;

		double priceYesterday = prices[i - 1].price;
		double priceToday = prices[i].price;
		double smaYesterday = sma[i - 1];
		double smaToday = sma[i];

		if (priceYesterday < smaYesterday && priceToday > smaToday) {
			if (!inPosition) {
				buyPrice = priceToday;
				inPosition = true;
				std::cout << prices[i].data << " - Покупка по $" << priceToday << std::endl;
			}
		}
		else if (priceYesterday > smaYesterday && priceToday < smaToday) {
			if (inPosition) {
				double sellPrice = priceToday;
				double tradeProfit = sellPrice - buyPrice;
				profit += tradeProfit;
				inPosition = false;
				std::cout << prices[i].data << " - Продажа по $" << sellPrice
					<< " (прибыль $" << tradeProfit << ")" << std::endl;
			}
		}
	}
	std::cout << "\nИтоговая прибыль: $" << profit << std::endl;

	return 0; 
}