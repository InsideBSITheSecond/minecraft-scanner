#pragma once

#include <new>
#include <string>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/make_shared.hpp>
#include <iostream>

#include <fmt/core.h>

#include "scanner.hpp"

namespace emss {

	class Scanner;

	class ThreadPool {
		friend class Scanner;
		public:
			std::size_t MAX_THREADS = 12;

			void faketask(std::string msg) {
				boost::this_thread::sleep_for(boost::chrono::milliseconds(rand() % 3000 + 1000));
				std::cout << msg << std::endl;
			}

			ThreadPool(std::size_t threadCount) : MAX_THREADS{threadCount} {
				io_service_ = boost::make_shared<boost::asio::io_service>();
				work_ = boost::make_shared<boost::asio::io_service::work>(*io_service_);

				std::cout << "created thread pool of size " << MAX_THREADS << std::endl;
				for (std::size_t i = 0; i < MAX_THREADS; ++i) {
					threadpool_.create_thread(boost::bind(&boost::asio::io_service::run, io_service_));
				}
			}

			~ThreadPool() { destroy(); }

			void pushChunkToThreadPool(Scanner *scanner) {
				io_service_->post(boost::bind(&Scanner::scanRegion, scanner));
			}

			void runFakeTasks(std::size_t jobTime, vec2 crds) {
				std::cout << "adding " << std::to_string(jobTime) << " jobs to the job pool" << std::endl;
				for (std::size_t i = 0; i < jobTime; ++i){
					io_service_->post(boost::bind(&ThreadPool::faketask, this, fmt::format("fake task {} finished ({} - {})", i, crds.x, crds.z)));
				}
			}

			void destroy() {
				io_service_->stop();
				threadpool_.join_all();
			}

		private:
			boost::shared_ptr<boost::asio::io_service> io_service_;
			boost::shared_ptr<boost::asio::io_service::work> work_;
			boost::thread_group threadpool_;
	};
}