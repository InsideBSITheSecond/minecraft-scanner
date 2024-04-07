#pragma once
#include <string>
#include <filesystem>
#include <format>
#include <unordered_map>
#include <iostream>

#include "region_file_reader.h"

#include <indicators/progress_bar.hpp>

namespace emss {
	enum REPORT_TYPE {
		DEBUG,
		USERFRIENDLY
	};

	struct vec2{
		int x;
		int z;
	};

	struct Progress{
		indicators::ProgressBar *region;
		indicators::ProgressBar *chunk;
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

			std::string worldPath_ = std::filesystem::current_path().string() + "/world";
			Progress bars;
		private:
			region_file_reader reader_;
			vec2 regioncrd_;
			std::vector<std::string> lookup_;
	};
}