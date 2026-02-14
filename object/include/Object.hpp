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
    template<typename Object> Object build_object(ObjInfo<Object> info)
    {
      return Object::create(info);
    }
  }  //namespace internal

  template<typename Object> struct ObjectPool {
    /**
     * @brief Get an object from the pool
     */
    void get(ObjInfo<Object> info);

    /**
     * @brief Retire an object from the pool
     */
    void release();

    size_t size() const { return this->last_; }

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

  template<typename Object>
  void ObjectPool<Object>::get(ObjInfo<Object> info)
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
      if (!(obj.isAlive())) {
        std::swap(obj, this->objects_[--this->last_]);
      }
    }
  }

}  //namespace kalika

#endif
