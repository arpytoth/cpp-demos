/*
 *  Virtual Destruction Synchronization Bug
 *  Copyright (C) 2016  Arpad Toth
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Arpad Toth <arpytoth@live.com>
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class A {
public:
  virtual void a_function() = 0;

  void done() {
    std::lock_guard<std::mutex> guard{mutex_};
    is_done = true;
    condition_.notify_one();
    std::cout << "Called something." << std::endl;
  }

  virtual ~A() {
    std::unique_lock<std::mutex> lock{mutex_};
    std::cout << "Waiting for something to be called." << std::endl;

    condition_.wait(lock, [&] { return is_done; });
    std::cout << "Destroying object." << std::endl;
  }

private:
  std::mutex mutex_;
  std::condition_variable condition_;
  bool is_done {false};
};

class B: public A {
  virtual void a_function() {}
  virtual ~B() {}
};

int main() 
{
  A* obj{new B()};

  std::thread t1{[=] {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      obj->a_function();
      obj->done();
  }};

  std::thread t2{[=] {
      delete obj;
  }};

  t1.join();
  t2.join();

  return 0;
}
