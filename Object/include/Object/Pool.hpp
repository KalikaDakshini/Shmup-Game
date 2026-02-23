#ifndef POOL_H
#define POOL_H

#include <memory>
#include <vector>

namespace kalika
{
  template<typename Object> struct Pool {
    // Delete all copy constructors
    Pool(Pool const&) = delete;
    Pool(Pool&&) = delete;
    Pool operator=(Pool const&) = delete;
    Pool operator=(Pool&&) = delete;

    using const_iterator = std::vector<Object>::const_iterator;

    /**
     * @brief Acquire an object from the pool. Creates a new object if none
     * are available
     */
    template<typename... Args> Object& acquire(Args... args);

    /**
     * @brief Mark the object inactive
     */
    void release(Object& obj);

    // Iterators
    const_iterator begin() const { return this->objects_.begin(); }

    const_iterator end() const
    {
      return this->objects_.begin() + this->last_;
    }

    /**
     * @brief Current capacity of the pool
     */
    [[nodiscard]] size_t capacity() { return this->objects_.size(); }

  private:
    std::vector<Object> objects_;
    size_t last_ = 0UL;
  };

  // Acquire an object from the pool
  template<typename Object>
  template<typename... Args>
  Object& Pool<Object>::acquire(Args... args)
  {
    if (this->last_ == this->objects_.size()) {
      return this->objects_.emplace_back(args...);
    }

    this->objects_[this->last_].rebuild(args...);
    return this->objects_[this->last_++];
  }

  // Mark an object unused
  template<typename Object> void Pool<Object>::release(Object& obj)
  {
    if (this->last_ == 0) {
      return;
    }

    std::swap(obj, this->objects_[--this->last_]);
  }

}  //namespace kalika

#endif
