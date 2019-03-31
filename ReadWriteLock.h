#ifndef READWRITELOCK_H
#define READWRITELOCK_H

#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

class ReadWriteLock {
public:
   virtual void acquire_read() = 0;
   virtual void release_read() = 0;

   virtual void acquire_write() = 0;
   virtual void release_write() = 0;
};

class ReadWriteLock1 : public ReadWriteLock {
public:
   ReadWriteLock1()
   : read_mutex_()
   , write_mutex_()
   , num_readers_(0)
   {
   }

   void acquire_read() {
      this->read_mutex_.lock();
      if (++this->num_readers_ == 1) {
         this->write_mutex_.lock();
      }
      this->read_mutex_.unlock();
   }

   void release_read() {
      this->read_mutex_.lock();
      if (--this->num_readers_ == 0) {
         this->write_mutex_.unlock();
      }
      this->read_mutex_.unlock();
   }

   void acquire_write() {
      this->write_mutex_.lock();
   }

   void release_write() {
      this->write_mutex_.unlock();
   }
   
private:
   std::mutex read_mutex_;
   std::mutex write_mutex_;
   std::atomic<int> num_readers_;
};

class ReadWriteLock2 : public ReadWriteLock {
public:
   ReadWriteLock2()
   : m_()
   , lock_available_()
   , writer_waiting_(false)
   , num_readers_(0)
   {
   }

   void acquire_read() {
      std::unique_lock<std::mutex> lck(this->m_);

      this->lock_available_.wait(
         lck, [this] () {
            return !this->writer_waiting_.load();
         }
      );
      ++this->num_readers_;
   }

   void release_read() {
      std::unique_lock<std::mutex> lck(this->m_);
      if (--this->num_readers_ == 0) {
         this->lock_available_.notify_all();
      }
   }

   void acquire_write() {
      std::unique_lock<std::mutex> lck(this->m_);

      this->lock_available_.wait(
         lck, [this] () {
            return !this->writer_waiting_.load() && this->num_readers_ == 0;
         }
      );
      this->writer_waiting_ = true;
   }

   void release_write() {
      std::unique_lock<std::mutex> lck(this->m_);
      
      this->writer_waiting_ = false;
      this->lock_available_.notify_all();
   }

private:
   std::mutex m_;
   std::condition_variable lock_available_;
   std::atomic<bool> writer_waiting_;
   std::atomic<int> num_readers_;
};

#endif
