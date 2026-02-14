#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/System.hpp>
#include <algorithm>
#include <deque>

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
    void add(ObjInfo<Object> info);

    /**
     * @brief Retire an object from the pool, i.e. despawn it
     */
    void retire();

    size_t size() const { return this->objects_.size(); }

    using iterator = std::deque<Object>::iterator;
    using const_iterator = std::deque<Object>::const_iterator;

    iterator begin() { return objects_.begin(); }

    iterator end() { return objects_.end(); }

    const_iterator begin() const { return objects_.begin(); }

    const_iterator end() const { return objects_.end(); }

  private:
    std::deque<Object> objects_;
  };

  template<typename Object>
  void ObjectPool<Object>::add(ObjInfo<Object> info)
  {
    this->objects_.emplace_back(internal::build_object<Object>(info));
  }

  // Retire an object in pool
  template<typename Object> void ObjectPool<Object>::retire()
  {
    std::erase_if(this->objects_, [](auto obj) {
      return !(obj.isAlive());
    });
  }

}  //namespace kalika

#endif
