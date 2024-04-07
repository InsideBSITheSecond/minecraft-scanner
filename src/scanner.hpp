#pragma once
#include <string>
#include <filesystem>
#include <format>
#include <unordered_map>
#include <iostream>

#include "region_file_reader.h"

namespace emss {
	enum REPORT_TYPE {
		DEBUG,
		USERFRIENDLY
	};

	struct vec2{
		vec2(int X, int Z) : x{X}, z{Z} {};

		int x;
		int z;
	};

	class FoundBlock : public Block {
		public:
			FoundBlock(Block from, vec2 reg, vec2 chun, vec2 blo) 
				: Block(from.getName(), from.getPos()),
				regioncrd{reg},
				chunkcrd{chun},
				blockcrd{blo} {

			};

			vec2 regioncrd;
			vec2 chunkcrd;
			vec2 blockcrd;
	};

	class Scanner {
		public:
			Scanner(vec2 regcrd, std::vector<std::string> table) : regioncrd_{regcrd} {
				openRegionFile();
				lookup_ = table;
			};

			void openRegionFile();
			void scanRegion();
			void scanChunk(vec2 crd);
			void checkBlock(FoundBlock *block);

			void writeReport(REPORT_TYPE type);
			
			std::unordered_map<std::string, std::vector<FoundBlock *>> foundMap;

		private:
			region_file_reader reader_;
			vec2 regioncrd_;
			std::string worldPath_ = std::filesystem::current_path().string() + "/world";
			std::vector<std::string> lookup_;
	};
}