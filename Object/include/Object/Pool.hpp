#ifndef POOL_H
#define POOL_H

#include <deque>
#include <limits>
#include <utility>

// Wrapper around the object
using slot_id = std::size_t;
constexpr size_t npos = std::numeric_limits<std::size_t>::max();

/**
 * @brief Wrapper around an object class
 */
template<typename Object> struct Wrapper {
  Object obj;
  slot_id idx;
  slot_id next_free = npos;
  bool occupied = false;

  // Constructor
  template<typename... Args>
  Wrapper(Args&&... args) : obj(std::forward<Args>(args)...)
  {}

  // Operator overloading
  Object* operator->() { return &(this->obj); }

  Object const* operator->() const { return &(this->obj); }
};

/**
 * @brief Generic Pool
 */
template<typename Object> struct Pool {
  /**
   * @brief Acquire the index of the object
   */
  template<typename... Args> Wrapper<Object>& acquire(Args&&... args)
  {
    // Pool is fill. Add a new object
    if (this->free_head_ == npos) {
      Wrapper<Object>& slot =
        this->slots_.emplace_back(std::forward<Args>(args)...);

      slot.idx = this->capacity() - 1;
      slot.occupied = true;
      return slot;
    }
    // Get the object at the free head
    slot_id idx = this->free_head_;
    Wrapper<Object>& slot = this->slots_[idx];

    // Rebuild its state
    slot->rebuild(std::forward<Args>(args)...);
    slot.occupied = true;
    this->free_head_ = slot.next_free;

    slot.next_free = npos;

    return slot;
  }

  /**
   * @brief Release the object for re-use
   */
  void release(slot_id idx)
  {
    // Check if index is valid
    if (!valid(idx)) {
      return;
    }

    Wrapper<Object>& slot = this->slots_[idx];
    // Set the slot to free
    slot.occupied = false;
    slot.next_free = free_head_;

    // Set the free head to current
    this->free_head_ = idx;
  }

  /**
   * @brief Capacity of the pool
   */
  slot_id capacity() const { return this->slots_.size(); }

private:
  slot_id free_head_ = npos;
  std::deque<Wrapper<Object>> slots_;

  // ======= Helper functions ======= //
  bool valid(slot_id idx) const
  {
    return idx < this->slots_.size() && slots_[idx].occupied;
  }
};

#endif
