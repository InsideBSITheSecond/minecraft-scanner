#include "scanner.hpp"

namespace emss {
	void Scanner::openRegionFile(){
		reader_ = region_file_reader(std::format("{}/region/r.{}.{}.mca", worldPath_, regioncrd_.x, regioncrd_.z));
		reader_.read();
	}

	void Scanner::scanRegion(){
		for(unsigned int chunkX = 0; chunkX < 32; ++chunkX) {
			for(unsigned int chunkZ = 0; chunkZ < 32; ++chunkZ) {
				int part = (chunkX*32 + chunkZ);
				int whole = (32*32);
				int progress = floor((float(part) / float(whole)) * 100);
				bars.chunk->set_progress(progress);
				bars.chunk->set_option(indicators::option::PostfixText{std::format("Scanning region {} {} chunk {} {}",
					regioncrd_.x, regioncrd_.z, chunkX, chunkZ)});  
				/*std::cout << std::format("{} {} - {} {} - {}",
					chunkX, chunkZ, part, whole, progress) << std::endl;*/
				scanChunk(vec2(chunkX, chunkZ));
			}
		}
	}

	void Scanner::scanChunk(vec2 crd){
		for (unsigned int x = 0; x < 16; ++x) {
			for (unsigned int z = 0; z < 16; ++z) {
				if(!reader_.is_filled(x, z))
					continue;

				std::vector<Block> blocks = reader_.get_blocks_at(crd.x, crd.z, x, z);
				
				for (Block block : blocks) {
					checkBlock(new FoundBlock(block, regioncrd_, crd, vec2(x, z)));
				}
			}
		}
	}

	void Scanner::checkBlock(FoundBlock *block) {
		for (std::string str : lookup_) {
			if (block->getName() == str) {
				foundMap[str].push_back(block);
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
		for (std::string item : lookup_) {
			std::ofstream outputfile(item + ".txt");
			if (outputfile.is_open()) {
				for (FoundBlock *block : foundMap[item]) {
					if (type == DEBUG) {
						outputfile << std::format("{} {} {}", block->getPos()[0], block->getPos()[1], block->getPos()[2]) << std::endl;
						outputfile << std::format("{} {} : {} {} : {} {}", 
							block->regioncrd.x, block->regioncrd.z,
							block->chunkcrd.x, block->chunkcrd.z,
							block->blockcrd.x, block->blockcrd.z)
							<< std::endl;
					}

					if (type == USERFRIENDLY) {
						outputfile << std::format("/tp @s {} {} {}", 
							block->getPos()[0], block->getPos()[1], block->getPos()[2]) 
							<< std::endl;
					}
					
				}
				outputfile.close();
			}
		}
	}
}