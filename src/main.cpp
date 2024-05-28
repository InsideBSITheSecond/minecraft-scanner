#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <cstdio>
#include <ctime>

#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/multi_progress.hpp>
#include <indicators/dynamic_progress.hpp>

#include "scanner.hpp"
#include "region_file_reader.h"

#include <fmt/core.h>
#include "threads.hpp"

void askQuestion(std::string question, std::string *str) {
	std::cout << question << std::endl;
	std::getline(std::cin, *str);
}

std::string getFilename(const std::string& filepath) {
    size_t lastSlashPos = filepath.find_last_of("/\\");
    size_t lastDotPos = filepath.find_last_of('.');

    if (lastDotPos != std::string::npos && lastDotPos > lastSlashPos + 1) {
        return filepath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    }

    return filepath.substr(lastSlashPos + 1);
}

vec2 getRegionCoord(const std::string& filename) {
    std::istringstream iss(filename);
    std::string token;
	vec2 coords;
    int count = 0;

    while (std::getline(iss, token, '.')) {
		if (token == "r")
			continue;

        if (count == 0) {
            coords.x = std::stoi(token);
        } else if (count == 1) {
            coords.z = std::stoi(token);
            break; // we got both integers, exit loop
        }
        count++;
    }

    return coords;
}

int main() {
	using namespace indicators;

	// Hide cursor
	show_console_cursor(false);

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

		std::string worldpath = "/home/insidebsi/mmosrvcpy/forge/world/";
		/*askQuestion("path to world (or leave blank for " + std::filesystem::current_path().string() + "/world" + ")",
			&worldpath);
		if (worldpath == "")
			worldpath = std::filesystem::current_path().string() + "/world";*/
		
		
		std::string list;
		askQuestion("Please enter a space-separated list of block names you are looking for in the format modname:blockname",
			&list);
		std::stringstream ss(list);
		std::vector<std::string> lookupTable;
		std::string word;
		while (ss >> word)
			lookupTable.push_back(word);

		auto dirIter = std::filesystem::directory_iterator(worldpath + "/region");
		int fileCount = std::count_if(
			begin(dirIter),
			end(dirIter),
			[](auto& entry) {return entry.is_regular_file();});

		ProgressBar globalProgress{
			option::BarWidth{50},
			option::Start{"["},
			option::Fill{"■"},
			option::Lead{"■"},
			option::Remainder{"-"},
			option::End{"]"},
			option::PostfixText{"region progress"},
			option::ForegroundColor{Color::cyan},
			option::ShowElapsedTime{true},
			option::ShowRemainingTime{true},
			option::MaxProgress{fileCount},
			option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
		};


		/*regionBar.set_progress(0);
		emss::Scanner scanner = emss::Scanner(emss::vec2(0,0), lookupTable, worldpath);
		scanner.worldPath_ = worldpath;
		scanner.bars = &bars;
		scanner.scanRegion();
		scanner.writeReport(emss::USERFRIENDLY);*/

		DynamicProgress<ProgressBar> progress{};
		progress.set_option(option::HideBarWhenComplete{true});

		emss::ThreadPool pool_{18};

		std::vector<emss::Scanner*> waitingScanners;
		std::vector<emss::Scanner*> runningScanners;
		std::vector<emss::Scanner*> finishedScanners;

		//scanner->bars = &bars;

		int gpIndex = progress.push_back(globalProgress);
		progress[gpIndex].set_progress(0);

		for (auto const &file : std::filesystem::directory_iterator{worldpath + "/region"}) {
			/*chunkBar.set_progress(0);
			regionBar.set_option(option::PostfixText{
				"region progress " + std::to_string(regionBar.current()) + "/" + std::to_string(fileCount)});*/

			std::string filename = getFilename(file.path());
			vec2 coords = getRegionCoord(filename);
			emss::Scanner *scanner = new emss::Scanner(lookupTable, worldpath, &waitingScanners, &runningScanners, &finishedScanners, &progress);
			scanner->worldPath_ = worldpath;
			scanner->regioncrd = coords;
			waitingScanners.push_back(scanner);
			pool_.pushChunkToThreadPool(scanner);

			//regionBar.set_progress(regionBar.current() + 1);
		}

		while (!waitingScanners.empty() || !runningScanners.empty()) {
			{
				
				progress[gpIndex].set_option(indicators::option::PostfixText{fmt::format("Global progress: {}% (W{} R{} F{})",
					finishedScanners.size() / fileCount * 100, waitingScanners.size(), runningScanners.size(), finishedScanners.size())});
					
				progress[gpIndex].set_progress((std::size_t)finishedScanners.size());
				for (emss::Scanner *scanner : runningScanners) {
					boost::lock_guard<boost::mutex> lock(scanner->mutex);
					progress[scanner->index].set_option(indicators::option::PostfixText{fmt::format("{}% region: [{} {}] chunk: [{} {}]", 
					scanner->percent, scanner->regioncrd.x, scanner->regioncrd.z, scanner->chunkX, scanner->chunkZ)});
					progress[scanner->index].set_progress(scanner->percent);
				}
			}
			boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
		}

		std::cout << "end" << std::endl;

		show_console_cursor(true);

	// catch all exception that may occur
	} catch(std::runtime_error &exc) {
		std::cerr << exc.what() << std::endl;
		show_console_cursor(true);
		return 1;
	}
	return 0;
}