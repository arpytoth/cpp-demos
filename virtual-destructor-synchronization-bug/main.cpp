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

/**
 * A simple class that has a pure virtual function defined.
 */
class A 
{
public:
  /**
   * The pure virtual function definition.
   */
  virtual void a_function() = 0;

  /**
   * Sets the done flag to true and allow destructor to be called.
   * The destructor will wait until done is called in another thread.
   */
  void done() 
  {
    std::lock_guard<std::mutex> guard{ mutex_ };
    is_done = true;
    condition_.notify_one();
    std::cout << "Called something." << std::endl;
  }

  /**
   * The destructor is virtual because we will extend this class.
   * This destructor will wait until you call the A::done() method.
   * It is a common scenario to wait for destruction of an object until
   * you release some resources etc..
   */
  virtual ~A() 
  {
    std::unique_lock<std::mutex> lock{ mutex_ };
    std::cout << "Waiting for something to be called." << std::endl;

    condition_.wait(lock, [&] { return is_done; });
    std::cout << "Destroying object." << std::endl;
  }

private:
  std::mutex mutex_;
  std::condition_variable condition_;
  bool is_done{ false };
};

/**
 * Class B implements class A and provides an empty implementation
 * of the pure virtual a_function() method.
 */
class B : public A 
{
  virtual void a_function() {}
  virtual ~B() {}
};

int main()
{
  A* obj{ new B() };

  std::thread t1
  { 
    [=] {
      // We delay the done() call to be sure that delete obj is called first.
      // Normally the delete obj call should wait for done to be called and should
      // then delete the object without any issues, because we synchronized the
      // destructor code.
      // The reality is that in C++ we can't safely syncrhonize a destructor. The vptr
      // access can't be synchronized and is vptr is actually DELETED when you call the destructor.
      // When obj->a_function() is called, the call will use the same vptr instance already
      // deleted, and the program will crash.
      std::this_thread::sleep_for(std::chrono::seconds(2));
      obj->a_function();
      obj->done(); } 
  };

  std::thread t2 { [=] { delete obj; } };

  t1.join();
  t2.join();

  return 0;
}
