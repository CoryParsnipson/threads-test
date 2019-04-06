#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <condition_variable>
#include <vector>

#include "ReadWriteLock.h"

bool go { false };

int main(int argc, char** argv) {
   std::condition_variable rw_start_cv;
   std::atomic<bool> rw_start { false };
   std::mutex rw_start_mutex;
   ReadWriteLock* rwl = new ReadWriteLock1();

   auto reader_f = [&rw_start_cv, &rw_start_mutex, &rw_start] (ReadWriteLock* rwl, int reader_id) {
      std::cout << "Initializing reader " << reader_id << std::endl;
      {
         std::unique_lock<std::mutex> lck(rw_start_mutex);
         rw_start_cv.wait(lck, [&rw_start] () { return rw_start.load(); });
      }

      std::cout << "Reader " << reader_id << " executing..." << std::endl;

      for (int j = 0; j < 10; ++j) {
         // random read delay
         std::this_thread::sleep_for(std::chrono::milliseconds(10 * (rand() % 1000)));

         // do the read
         rwl->acquire_read();
         std::cout << "Reader " << reader_id << " doing a read now..." << std::endl;
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         std::cout << "Reader " << reader_id << " done..." << std::endl;
         rwl->release_read();
      }
   };

   auto writer_f = [&rw_start_cv, &rw_start_mutex, &rw_start] (ReadWriteLock* rwl, int writer_id) {
      std::cout << "Initializing writer " << writer_id << std::endl;
      {
         std::unique_lock<std::mutex> lck(rw_start_mutex);
         rw_start_cv.wait(lck, [&rw_start] () { return rw_start.load(); });
      }

      std::cout << "Writer " << writer_id << " executing..." << std::endl;

      for (int j = 0; j < 10; ++j) {
         // random read delay
         std::this_thread::sleep_for(std::chrono::milliseconds(10 * (rand() % 1000)));

         // do the write
         rwl->acquire_write();
         std::cout << "Writer " << writer_id << " doing a write now..." << std::endl;
         std::this_thread::sleep_for(std::chrono::milliseconds(1500));
         std::cout << "Writer " << writer_id << " done..." << std::endl;
         rwl->release_write();
      }
   };

   std::vector<std::thread> readers;
   std::vector<std::thread> writers;

   for (int i = 0; i < 10; ++i) {
      readers.push_back(std::thread(reader_f, rwl, i));
      writers.push_back(std::thread(writer_f, rwl, i));
   }

   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   // start reading/writing
   rw_start = true;
   rw_start_cv.notify_all();

   for (int i = 0; i < 10; ++i) {
      readers[i].join();
      writers[i].join();
   }

   return 0;
}
