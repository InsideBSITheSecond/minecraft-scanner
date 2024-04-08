#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>
#include <iostream>

#include "region_file_reader.h"

#include <fmt/core.h>
#include <indicators/progress_bar.hpp>
#include <indicators/multi_progress.hpp>

#include <fmt/core.h>

namespace emss {
	using namespace indicators;
	
	enum REPORT_TYPE {
		DEBUG,
		USERFRIENDLY
	};

	struct vec2{
		int x;
		int z;
	};

	struct Progress{
		ProgressBar *region;
		ProgressBar *chunk;
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
			Scanner(vec2 regcrd, std::vector<std::string> table, std::string path) : regioncrd_{regcrd}, worldPath_{path} {
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
			MultiProgress<ProgressBar, 2> *bars;
		private:
			region_file_reader reader_;
			vec2 regioncrd_;
			std::vector<std::string> lookup_;
	};
}