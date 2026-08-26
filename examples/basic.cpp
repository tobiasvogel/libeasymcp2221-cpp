/**
 * @file basic.cpp
 * @brief Minimal libeasymcp2221++ construction example.
 */

#include <libeasymcp2221++/libeasymcp2221++.h>

#include <iostream>

int main() {
	try {
		libeasymcp2221::Device device;
		std::cout << "MCP2221 opened successfully\n";
		return 0;
	} catch (const libeasymcp2221::Error& error) {
		std::cerr << "MCP2221 error: " << error.what() << '\n';
		return 1;
	}
}
