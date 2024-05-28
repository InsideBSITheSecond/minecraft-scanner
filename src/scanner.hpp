#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <iostream>

#include "region_file_reader.h"

#include <fmt/core.h>
#include <indicators/progress_bar.hpp>
#include <indicators/multi_progress.hpp>
#include <indicators/dynamic_progress.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/make_shared.hpp>

#include "types.h"

namespace emss {
	using namespace indicators;

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
			Scanner(std::vector<std::string> table, std::string path, std::vector<emss::Scanner*> *wait, std::vector<emss::Scanner*> *run, std::vector<emss::Scanner*> *end, DynamicProgress<ProgressBar> *progress)
			: worldPath_{path}, lookup_{table}, waitingScanners_{wait}, runningScanners_{run}, finishedScanners_{end}, progress{progress} {

			};

			~Scanner() {

			};

			void openRegionFile();
			void scanRegion();
			void scanChunk(vec2 crd);
			void checkBlock(Block block, vec2 chunkcrd, vec2 blockcrd);

			void writeReport(REPORT_TYPE type);
			
			std::unordered_map<std::string, std::vector<FoundBlock *>> foundMap;
			std::unordered_map<std::string, unsigned long long> foundCount;

			std::string worldPath_ = std::filesystem::current_path().string() + "/world";

			DynamicProgress<ProgressBar> *progress;

			ProgressBar bar{
				option::BarWidth{50},
				option::Start{"["},
				option::Fill{"■"},
				option::Lead{"■"},
				option::Remainder{"-"},
				option::End{"]"},
				option::PostfixText{"chunk progress"},
				option::ForegroundColor{Color::cyan},
				option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
			};

			boost::mutex mutex;

			int part = 0;
			int whole = 0;
			size_t percent = 0;
			int index = 0;

			vec2 regioncrd;
			unsigned int chunkX;
			unsigned int chunkZ;
		private:
			region_file_reader *reader_;
			std::vector<std::string> lookup_;

			std::vector<emss::Scanner*> *waitingScanners_;
			std::vector<emss::Scanner*> *runningScanners_;
			std::vector<emss::Scanner*> *finishedScanners_;
	};
}