#include "scanner.hpp"

namespace emss {
	void Scanner::openRegionFile(vec2 regioncrd){
		regioncrd_ = regioncrd;
		reader_ = region_file_reader(fmt::format("{}/region/r.{}.{}.mca", worldPath_, regioncrd.x, regioncrd.z));
		reader_.read();
	}

	void Scanner::scanRegion(){
		for(unsigned int chunkX = 0; chunkX < 32; ++chunkX) {
			for(unsigned int chunkZ = 0; chunkZ < 32; ++chunkZ) {
				int part = (chunkX*32 + chunkZ);
				int whole = (32*32);
				size_t progress = floor((float(part) / float(whole)) * 100);
				bars->set_progress<0>(progress);
				//bars->set_option<0>(indicators::option::PostfixText{fmt::format("Scanning chunk {} {}",
				//	chunkX, chunkZ)});  
				/*std::cout << fmt::format("{} {} - {} {} - {}",
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
					checkBlock(block, crd, vec2(x, z));
				}
			}
		}
	}

	void Scanner::checkBlock(Block block, vec2 chunkcrd, vec2 blockcrd) {
		foundCount[block.getName()]++;
		for (std::string str : lookup_) {
			if (block.getName() == str) {
				foundMap[str].push_back(new FoundBlock(block, regioncrd_, chunkcrd, blockcrd));
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