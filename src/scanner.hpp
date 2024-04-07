#pragma once
#include <string>
#include <filesystem>
#include <format>
#include <unordered_map>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec2.hpp"

#include "region_file_reader.h"

namespace emss {
	enum REPORT_TYPE {
		DEBUG,
		USERFRIENDLY
	};

	class FoundBlock : public Block {
		public:
			FoundBlock(Block from, glm::vec2 reg, glm::vec2 chun, glm::vec2 blo) 
				: Block(from.getName(), from.getPos()),
				regioncrd{reg},
				chunkcrd{chun},
				blockcrd{blo} {

			};

			glm::vec2 regioncrd;
			glm::vec2 chunkcrd;
			glm::vec2 blockcrd;
	};

	class Scanner {
		public:
			Scanner(glm::vec2 regcrd, std::vector<std::string> table) : regioncrd_{regcrd} {
				openRegionFile();
				lookup_ = table;
			};

			void openRegionFile();
			void scanRegion();
			void scanChunk(glm::vec2 crd);
			void checkBlock(FoundBlock *block);

			void writeReport(REPORT_TYPE type);
			
			std::unordered_map<std::string, std::vector<FoundBlock *>> foundMap;

		private:
			region_file_reader reader_;
			glm::vec2 regioncrd_;
			std::string worldPath_ = std::filesystem::current_path().string() + "/world";
			std::vector<std::string> lookup_;
	};
}