#ifndef MEM_UNIQUE_PTR_H
#define MEM_UNIQUE_PTR_H

#include <memory>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

template <typename data_t>
class mem_unique_ptr
{
  using value_type      = data_t; 
  using reference       = data_t&;
  using const_reference = const data_t&;
  using pointer         = data_t*;
  using const_pointer   = const data_t*;

public:
  /***/
  constexpr mem_unique_ptr( ) noexcept
    : _ptr( nullptr ), _auto_delete( true )
  { }

  constexpr mem_unique_ptr( std::nullptr_t ) noexcept
    : _ptr( nullptr ), _auto_delete( true )
  { }

  /***/
  template<Derived<value_type> T>
  constexpr mem_unique_ptr( const mem_unique_ptr<T>& ptr ) noexcept = delete;

  /***/
  template<Derived<value_type> T>
  constexpr mem_unique_ptr( T* ptr, bool autodelete = true ) noexcept
    : _ptr( ptr ), _auto_delete( autodelete )
  { }

  /***/
  template<Derived<value_type> T>
  constexpr mem_unique_ptr( std::unique_ptr<T>&& ptr, bool autodelete = true ) noexcept
    : _ptr( ptr.release() ), _auto_delete( autodelete )
  { }

  /***/
  template<Derived<value_type> T>
  constexpr mem_unique_ptr( mem_unique_ptr<T>&& ptr ) noexcept
    : _ptr( ptr.release() ), _auto_delete( ptr.auto_delete() )
  { }

  /***/
  constexpr ~mem_unique_ptr() noexcept
  {
    if ((_auto_delete == true) && ( _ptr != nullptr ))
    {
      delete _ptr;
      _ptr = nullptr;
    }
  }

  /***/
  template<Derived<value_type> T>
  constexpr reference operator=( const mem_unique_ptr<T>& ptr ) noexcept = delete;

  /***/
  template<Derived<value_type> T>
  constexpr reference operator=( std::unique_ptr<T>&& ptr ) noexcept
  { 
    _ptr           = ptr.release();
    _auto_delete   = true;
    return *this; 
  }

  /***/
  template<Derived<value_type> T>
  constexpr reference operator=( mem_unique_ptr<T>&& ptr ) noexcept
  { 
    _ptr           = ptr.release();
    _auto_delete   = ptr.auto_delete();
    return *this; 
  }

  /***/
  constexpr reference  operator*() const noexcept
  {
    static_assert(get() != pointer());
    return *get();
  }

  /***/
  constexpr pointer operator->() const noexcept
  { return get(); }

  /***/
  constexpr pointer get() const noexcept
  { return _ptr; }

  /***/
  constexpr bool    auto_delete() const noexcept
  { return _auto_delete; }

  /* Return true if the stored pointer is not null. */
  constexpr operator bool() const noexcept
  { return get() == pointer() ? false : true; }

  constexpr pointer release() noexcept
  { 
    pointer ret_ptr = _ptr;
    _ptr = nullptr;
    return ret_ptr;
  }

private:
  pointer  _ptr;
  bool     _auto_delete;
};

template<typename data_t>
bool operator==( const mem_unique_ptr<data_t>& lhs, const mem_unique_ptr<data_t>& rhs)
{ return (lhs.get() == rhs.get()); }

template<typename data_t>
bool operator==(const mem_unique_ptr<data_t>& lhs, std::nullptr_t ) noexcept
{ return !(bool)lhs; }

template<typename data_t>
bool operator!=( const mem_unique_ptr<data_t>& lhs, const mem_unique_ptr<data_t>& rhs)
{ return (lhs.get() != rhs.get()); }

template<typename data_t>
bool operator!=(const mem_unique_ptr<data_t>& lhs, std::nullptr_t ) noexcept
{ return (bool)lhs; }

#endif //MEM_UNIQUE_PTR_H