#ifndef POOL_H
#define POOL_H

#include <format>
#include <iostream>
#include <memory>
#include <vector>

namespace kalika
{
  struct Handle {
    size_t idx;
  };

  template<typename Object> struct Pool {
    // Delete all copy constructors
    Pool() = default;
    Pool(Pool const&) = delete;
    Pool(Pool&&) = delete;
    Pool operator=(Pool const&) = delete;
    Pool operator=(Pool&&) = delete;

    using iterator = std::vector<Object>::iterator;
    using const_iterator = std::vector<Object>::const_iterator;

    /**
     * @brief Release an object from the pool to the user. Creates one if
     * not present
     */
    template<typename... Args>
    std::unique_ptr<Object> release(Args... args);

    /**
     * @brief Add the object back to pool
     */
    void acquire(size_t idx);

    // Iterators
    iterator begin() { return this->objects_.begin(); }

    iterator end() { return this->objects_.begin() + this->last_; }

    const_iterator begin() const { return this->objects_.begin(); }

    const_iterator end() const
    {
      return this->objects_.begin() + this->last_;
    }

    /**
     * @brief Current capacity of the pool
     */
    [[nodiscard]] size_t capacity() const { return this->objects_.size(); }

  private:
    std::vector<std::unique_ptr<Object>> objects_;
    size_t last_ = 0UL;
  };

  // Acquire an object from the pool
  template<typename Object>
  template<typename... Args>
  std::unique_ptr<Object> Pool<Object>::release(Args... args)
  {
    // Add object if pool is full
    if (this->last_ == this->objects_.size()) {
      this->objects_.emplace_back(args...);
    }
    // Get first unused object otherwise
    else {
      this->objects_[this->last_].rebuild(args...);
    }
    std::cout << std::format("Spawning bullet {}\n", this->last_);

    // Return a reference to the object
    return this->objects_[this->last_++];
  }

  // TODO(kalika): Implement an O(1) pool using Handles
  // Mark an object unused
  template<typename Object> void Pool<Object>::acquire(size_t idx)
  {
    std::cout << std::format("Despawning bullet {}\n", idx);
    // Invalid index
    if (idx >= this->last_) {
      return;
    }

    // Bug: Indices aren't swapped
    std::swap(this->objects_[idx], this->objects_[--this->last_]);
  }

}  //namespace kalika

#endif
