/**************************************************************************************************
 * 
 * Copyright 2022 https://github.com/fe-dagostino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following 
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 *************************************************************************************************/

#ifndef CSV_DATA_H
#define CSV_DATA_H

#include "csv_common.h"
#include <string.h>

namespace csv {
inline namespace LIB_VERSION {


template<typename data_t, typename data_size_t, data_size_t chunk_size = to_bytes<128>::Bytes>
class csv_data
{
public:
  typedef data_t             value_type; 
  typedef data_size_t        size_type;
  typedef data_t&            reference;
  typedef const data_t&      const_reference;
  typedef data_t*            pointer;
  typedef const data_t*      const_pointer;

  static const size_type data_type_size = sizeof(value_type);

  class iterator
  {
    public:
      typedef std::forward_iterator_tag iterator_category;

      constexpr iterator(pointer ptr) 
      : m_ptr(ptr)
      { }

      constexpr ~iterator() 
      { }

      // Prefix ++
      constexpr iterator& operator++() noexcept
      { m_ptr++; return *this; }
      // Prefix --
      constexpr iterator& operator--() noexcept
      { m_ptr--; return *this; }

      // Postfix ++
      constexpr iterator  operator++(int) noexcept
      { iterator _iter = *this; this->m_ptr++; return _iter; }
      // Postfix --
      constexpr iterator  operator--(int) noexcept
      { iterator _iter = *this; this->m_ptr--; return _iter; }

      /***/
      constexpr reference operator*() noexcept
      { return *m_ptr; }
      /***/
      constexpr pointer   operator->() noexcept
      { return m_ptr; }

      /***/
      constexpr bool      operator==(const iterator& rhs) noexcept
      { return m_ptr == rhs.m_ptr; }
      /***/
      constexpr bool      operator!=(const iterator& rhs) noexcept
      { return m_ptr != rhs.m_ptr; }

    private:
      pointer m_ptr;
  };


  class const_iterator
  {
    public:
      typedef std::forward_iterator_tag iterator_category;

      constexpr const_iterator(pointer ptr) 
      : m_ptr(ptr)
      { }
      constexpr ~const_iterator() 
      { }

      // Prefix ++
      constexpr const const_iterator& operator++() const noexcept
      { m_ptr++; return *this; }
      // Prefix --
      constexpr const const_iterator& operator--() const noexcept
      { m_ptr--; return *this; }

      // Postfix ++
      constexpr const const_iterator  operator++(int) const noexcept
      { iterator _iter = *this; this->m_ptr++; return _iter; }
      // Postfix --
      constexpr const const_iterator  operator--(int) const noexcept
      { iterator _iter = *this; this->m_ptr--; return _iter; }

      /***/
      constexpr const_reference       operator*() const noexcept
      { return *m_ptr; }
      /***/
      constexpr const_pointer         operator->() const noexcept
      { return m_ptr; }

      /***/
      constexpr bool                  operator==(const const_iterator& rhs) const noexcept
      { return m_ptr == rhs.m_ptr; }
      /***/
      constexpr bool                  operator!=(const const_iterator& rhs) const noexcept
      { return m_ptr != rhs.m_ptr; }

    private:
      const_pointer m_ptr;
  };

public:
  /***/
  constexpr csv_data()
    : m_pData(nullptr),
      m_nMaxSize(0),
      m_nLength(0)
  { }

  /**
   * @brief csv_data copy constructor do not copy exactly the origial object, 
   *        instead it allocate only required memory to 
   */
  constexpr csv_data( const csv_data& data )
    : csv_data()
  {
    if ( data.empty() == false )
    {
      resize( data.length()+1 );
      memcpy( this->data(), data.data(), data.size() );
      m_nLength  = data.length();
    }
  }

  constexpr csv_data( const_pointer buffer, size_type length )
    : csv_data()
  {
    if ((buffer != nullptr) && (length > 0))
    {
      resize( length+1 );
      memcpy( this->data(), buffer, length*data_type_size );
      m_nLength  = length;
    }
  }

  /***/
  constexpr csv_data( csv_data&& data ) 
  {
    // Use move assign operator in order
    *this = std::move(data);
  }

  constexpr ~csv_data()
  { release(); }

  /**
   * @brief  Retrieve the size in bytes required by current length.
   *         Note: if sizeof(data_t) == 1 then size() and length() 
   *               will have the same value.
   * 
   * @return current size in bytes for current length. 
   */
  constexpr size_type  size() const noexcept
  { return (this->length()*data_type_size); }
  /***/
  constexpr size_type  length() const noexcept
  { return m_nLength; }

  /**
   * @brief Retrieve return maximum number of data_t that can be 
   *        stored with current data buffer.
   * 
   * @return max allow data_t for current data buffer. 
   */
  constexpr size_type  max_size() const noexcept
  { return m_nMaxSize; }

  /**
   * @brief  Retrieve the size in bytes currently allocated.
   *         Note: if sizeof(data_t) == 1 then capacity() and max_size() 
   *               return the same value.
   * 
   * @return current size for the allocated buffer. 
   */
  constexpr size_type capacity() const noexcept
  { return (max_size() * data_type_size); }

  /**
   * @brief Resizes the data buffer to a length of n data_t.
   * 
   * If n is smaller than the current length, the current value is 
   * shortened to its first n data_t, removing all data_t beyond the nth.
   * 
   * If n is greater than the current length, the current content is 
   * extended by inserting at the end as many bytes as needed to reach a 
   * size of n. If c is specified, the new elements are initialized as copies 
   * of c, otherwise, they are value-initialized characters (null characters). 
   */
  constexpr void resize( size_type length ) noexcept
  {
    if (length == max_size())
      return;

    pointer pNewData = static_cast<pointer>(realloc( m_pData, length * data_type_size ));
    if ( pNewData != nullptr ) {
      m_pData     = pNewData;

      if ( length > max_size() ) {
        memset( m_pData+this->length(), '\0', (length-this->length()) * data_type_size );
      } else {
        // In this case previous buffer has been truncated and then last
        // character in the buffer will be set to '\0'
        memset( m_pData+(length-1), '\0', data_type_size );
      }

      m_nMaxSize = length;
    } else {
      release();
    }
  }

  /**
   * @brief Resizes the buffer to a length of n data_t.
   * 
   * If n is smaller than the current data_t length, the current value is 
   * shortened to its first n character, removing the characters beyond the nth.
   * 
   * If n is greater than the current data_t length, the current content is 
   * extended by inserting at the end as many characters as needed to reach a 
   * size of n. If c is specified, the new elements are initialized as copies 
   * of c, otherwise, they are value-initialized characters (null characters). 
   */
  constexpr void reserve( size_type length ) noexcept
  { resize(length); }

  /**
   * @brief Set the length to 0 items, however the capacity for the buffer will be
   *        the same and no memory will be deallocated.
   *        If you want to release also all internal buffer then call release() 
   *        method instead.
   */
  constexpr void clear() noexcept
  { 
    memset( m_pData, '\0', this->capacity() );
    m_nLength = 0; 
  }

  constexpr bool empty() const noexcept
  { return (length()==0); }


  /**
   * Append data_t to buffer.
   */
  constexpr void push_back( const_reference value ) noexcept
  {
    if (m_nLength+1 >= max_size()) {
      resize( max_size() + chunk_size );
    }

    m_pData[m_nLength++] = value;
  }

  /**
   *  @brief  Remove the last data_t if the buffer contain at least 
   *          one data_t.
   */
  constexpr void pop_back() noexcept
  {
    if( empty() == 0 ){
      memset( &m_pData[m_nLength--], '\0', data_type_size );
    }
  }

  /**
   *  @brief  Return const pointer to contents.
   *
   *  This is a pointer to internal data.  It is undefined to modify
   *  the contents through the returned pointer. 
  */
  constexpr const data_t*  c_str() const
  { return data(); }

  /**
   *  @brief  Return const pointer to contents.
   *
   *  This is a pointer to internal data.  It is undefined to modify
   *  the contents through the returned pointer. 
  */
  constexpr const data_t*  data() const noexcept
  { return static_cast<const_pointer>(m_pData); }

  /**
   *  @brief  Return non-const pointer to contents.
   *
   *  This is a pointer to the data_t sequence held by the csv_data.
   *  Modifying the data_t in the sequence is allowed.
  */
  constexpr data_t*        data() noexcept
  { return m_pData; }

  /***/
  constexpr iterator       begin() noexcept
  { return iterator( data() ); }
  /***/
  constexpr iterator       end() noexcept
  { return iterator( data()+max_size() ); }

  /***/
  constexpr const_iterator begin() const noexcept
  { return iterator( data() ); }
  /***/
  constexpr const_iterator end() const noexcept
  { return iterator( data()+max_size() ); }


  /***/
  constexpr csv_data&       operator=( csv_data&& data ) noexcept
  { 
    m_pData         = data.m_pData;
    data.m_pData    = nullptr;

    m_nMaxSize      = data.m_nMaxSize;
    data.m_nMaxSize = size_type(0);

    m_nLength       = data.m_nLength;
    data.m_nLength  = size_type(0);

    return *this; 
  }
   
  /**
   * @brief Release all internal memory and reset max_size() and length() 
   *        to ZERO.
   */
  constexpr void  release() noexcept
  {
    if ( m_pData != nullptr )
    {
      free (m_pData);
      m_pData     = nullptr;
      m_nMaxSize  = 0;
      m_nLength   = 0;
    }
  }

private:
  pointer      m_pData;
  size_type    m_nMaxSize;
  size_type    m_nLength;
  
};


} //inline namespace
} // namespace

#endif //CSV_DATA_H
