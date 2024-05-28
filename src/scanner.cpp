#include "scanner.hpp"

namespace emss {
	void Scanner::openRegionFile(){
		reader_ = new region_file_reader(fmt::format("{}/region/r.{}.{}.mca", worldPath_, regioncrd.x, regioncrd.z));
		reader_->read();
	}

	void Scanner::scanRegion(){
		//std::cout << "region " << regioncrd_.x << " " << regioncrd_.z << " start" << std::endl;
		index = progress->push_back(bar);
		openRegionFile();
		{
			boost::lock_guard<boost::mutex> lock(mutex);
			waitingScanners_->erase(
				std::find(waitingScanners_->begin(),
				waitingScanners_->end(),
				this));

			runningScanners_->push_back(this);
		}

		for(chunkX = 0; chunkX < 32; ++chunkX) {
			for(chunkZ = 0; chunkZ < 32; ++chunkZ) {
				scanChunk(vec2(chunkX, chunkZ));

				boost::lock_guard<boost::mutex> lock(mutex);
				part = (chunkX*32 + chunkZ);
				whole = (32*32);
				percent = floor((float(part) / float(whole)) * 100);
			}
			//(*progress)[index].set_option(indicators::option::PostfixText{fmt::format("Scanning chunk {}:0->32", chunkX)});
			//(*progress)[index].set_progress(percent);
		}
		{
			boost::lock_guard<boost::mutex> lock(mutex);
			runningScanners_->erase(
				std::find(runningScanners_->begin(),
				runningScanners_->end(),
				this));

			finishedScanners_->push_back(this);
			(*progress)[index].mark_as_completed();
		}
		writeReport(USERFRIENDLY);
		//std::cout << "region " << regioncrd_.x << " " << regioncrd_.z << " stop" << std::endl;
	}

	void Scanner::scanChunk(vec2 crd){
		for (unsigned int x = 0; x < 16; ++x) {
			for (unsigned int z = 0; z < 16; ++z) {
				if(!reader_->is_filled(x, z))
					continue;

				std::vector<Block> blocks = reader_->get_blocks_at(crd.x, crd.z, x, z);
				
				for (Block block : blocks) {
					checkBlock(block, crd, vec2(x, z));
				}
			}
		}
	}

	void Scanner::checkBlock(Block block, vec2 chunkcrd, vec2 blockcrd) {
		std::string blockName = block.getName();
		foundCount[blockName]++;
		for (std::string str : lookup_) {
			if (blockName == str) {
				foundMap[str].push_back(new FoundBlock(block, regioncrd, chunkcrd, blockcrd));
			}
		}
	}

	static const std::string getDaytime() {
		std::time_t rawtime;
		std::tm* timeinfo;
		char buffer [80];
		std::time(&rawtime);
		timeinfo = std::localtime(&rawtime);
		std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
		return buffer;
	}

	void Scanner::writeReport(REPORT_TYPE type) {
		for (auto kv : foundMap) {
			std::ofstream outputfile(kv.first + ".txt", std::ios_base::app);
			if (outputfile.is_open()) {
				for (FoundBlock *block : foundMap[kv.first]) {
					if (type == DEBUG) {
						outputfile << fmt::format("{} {} {}", block->getPos()[0], block->getPos()[1], block->getPos()[2]) << std::endl;
						outputfile << fmt::format("{} {} : {} {} : {} {}", 
							block->regioncrd.x, block->regioncrd.z,
							block->chunkcrd.x, block->chunkcrd.z,
							block->blockcrd.x, block->blockcrd.z)
							<< std::endl;
					}

					if (type == USERFRIENDLY) {
						outputfile << fmt::format("/tp @s {} {} {}", 
							block->getPos()[0], block->getPos()[1], block->getPos()[2]) 
							<< std::endl;
					}
					
				}
				outputfile.close();
			}
		}

		std::ofstream outputfile("count.txt", std::ios_base::app);
		if (outputfile.is_open()) {
			for (auto kv : foundCount) {
				outputfile << fmt::format("{} : {}", 
					kv.first, kv.second)
					<< std::endl;
			}
			outputfile.close();
		}
	}
}