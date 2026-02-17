#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/System.hpp>
#include <algorithm>
#include <iterator>
#include <vector>

#include "helpers.hpp"

namespace kalika
{
  namespace internal
  {
    template<typename Object> Object build_object(ObjInfo const& info)
    {
      return Object::create(info);
    }
  }  //namespace internal

  template<typename Object> struct ObjectPool {
    /**
     * @brief Get an object from the pool
     */
    void get(ObjInfo info);

    /**
     * @brief Retire an object from the pool
     */
    void release();

    /**
     * @brief Number of active objects in the pool
     */
    size_t size() const { return this->last_; }

    /**
     * @brief Return the current size of the pool
     */
    size_t capacity() const { return this->objects_.size(); }

    using iterator = std::vector<Object>::iterator;
    using const_iterator = std::vector<Object>::const_iterator;

    iterator begin() { return this->objects_.begin(); }

    iterator end() { return this->objects_.begin() + this->last_; }

    const_iterator begin() const { return this->objects_.begin(); }

    const_iterator end() const
    {
      return this->objects_.begin() + this->last_;
    }

  private:
    std::vector<Object> objects_;
    size_t last_ = 0UL;
  };

  // Get an object from pool, adding if there are no objects
  template<typename Object> void ObjectPool<Object>::get(ObjInfo info)
  {
    // Add a new object if pool is full
    if (this->last_ == this->objects_.size()) {
      this->objects_.emplace_back(internal::build_object<Object>(info));
      this->last_++;
      return;
    }

    // Update an unused object otherwise
    auto& obj = this->objects_[this->last_];
    obj.rebuild(info);
    this->last_++;
  }

  // Implement pool functionality
  template<typename Object> void ObjectPool<Object>::release()
  {
    auto idx = 0UL;
    while (idx < this->last_) {
      auto& obj = this->objects_[idx++];
      // Swap retired objects with the last active object
      if (!(obj.is_alive())) {
        std::swap(obj, this->objects_[--this->last_]);
      }
    }
  }

}  //namespace kalika

#endif
