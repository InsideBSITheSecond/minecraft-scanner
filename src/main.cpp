#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <cstdio>
#include <ctime>

#include "scanner.hpp"
#include "region_file_reader.h"

int main() {
	try {

		/*std::vector<std::string> diagnosticBlocks = {
				"minecraft:red_terracotta",
				"minecraft:green_terracotta",
				"minecraft:blue_terracotta",
				"minecraft:black_terracotta",

				"minecraft:iron_block",
				"minecraft:lapis_block",

				"minecraft:red_wool",
				"minecraft:green_wool",
				"minecraft:blue_wool",
				"minecraft:yellow_wool",
		};*/

		std::string userInput;
		std::cout << "Please enter a space-separated list of block names you are looking for in the format modname:blockname" << std::endl;
		std::getline(std::cin, userInput);
		std::stringstream ss(userInput);
		std::vector<std::string> lookupTable;
		std::string word;
		while (ss >> word)
			lookupTable.push_back(word);

		emss::Scanner *scanner = new emss::Scanner(glm::vec2(0, 0), lookupTable);

		scanner->scanRegion();
		scanner->writeReport();
		
		std::cout << "end" << std::endl;

	// catch all exception that may occur
	} catch(std::runtime_error &exc) {
		std::cerr << exc.what() << std::endl;
		return 1;
	}
	return 0;
}