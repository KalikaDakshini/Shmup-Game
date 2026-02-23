#ifndef POOL_H
#define POOL_H

#include <format>
#include <iostream>
#include <memory>
#include <vector>

namespace kalika
{
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
     * @brief Acquire an object from the pool. Creates a new object if none
     * are available
     */
    template<typename... Args> Object& acquire(Args... args);

    /**
     * @brief Mark the object inactive
     */
    void release(size_t idx);

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
    std::vector<Object> objects_;
    size_t last_ = 0UL;
  };

  // Acquire an object from the pool
  template<typename Object>
  template<typename... Args>
  Object& Pool<Object>::acquire(Args... args)
  {
    // Add object if pool is full
    if (this->last_ == this->objects_.size()) {
      this->objects_.emplace_back(this->last_, args...);
    }
    // Get first unused object otherwise
    else {
      this->objects_[this->last_].rebuild(this->last_, args...);
    }
    std::cout << std::format("Spawning bullet {}\n", this->last_);

    // Return a reference to the object
    return this->objects_[this->last_++];
  }

  // Mark an object unused
  template<typename Object> void Pool<Object>::release(size_t idx)
  {
    std::cout << std::format("Despawning bullet {}\n", idx);
    // Invalid index
    if (idx >= this->last_) {
      return;
    }

    std::swap(this->objects_[idx], this->objects_[--this->last_]);
  }

}  //namespace kalika

#endif
