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
			Scanner(std::vector<std::string> table, std::string path) : worldPath_{path}, lookup_{table} {
				
			};

			~Scanner() {

			};

			void openRegionFile(vec2 regioncrd);
			void scanRegion();
			void scanChunk(vec2 crd);
			void checkBlock(Block block, vec2 chunkcrd, vec2 blockcrd);

			void writeReport(REPORT_TYPE type);
			
			std::unordered_map<std::string, std::vector<FoundBlock *>> foundMap;
			std::unordered_map<std::string, unsigned long long> foundCount;

			std::string worldPath_ = std::filesystem::current_path().string() + "/world";
			MultiProgress<ProgressBar, 2> *bars;
		private:
			region_file_reader reader_;
			vec2 regioncrd_;
			std::vector<std::string> lookup_;
	};
}