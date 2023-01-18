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

#include "csv_parser.h"
#include <cstring>

namespace csv {
inline namespace CSV_LIB_VERSION {

csv_parser::csv_parser( const std::string& feedname )
  : csv_parser( feedname, nullptr, nullptr )
{
}

csv_parser::csv_parser( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents )
  : csv_base( feedname, std::move(ptrDevice), std::move(ptrEvents) ),
    m_eState (Status::eStart),
    m_sWhitespaces("\a\b\t\v\f\r\n"),
    m_bSkipWhitespaces(true),
    m_bTrimAll(true),
    m_bAllowComments(false),
    m_recvCachedBytes( 0 ),
    m_recvCacheCursor( 0 ),
    m_nRowsCounter( 0 )
{
}

csv_result csv_parser::parse_row( csv_row& row ) noexcept
{
  char          _ch{'\0'};
  csv_result    _retVal = csv_result::_ok;

  bool          _bEoL       = false;
  bool          _bAddField  = false;
  bool          _bSkipLine  = false;
  bool          _bQuoted    = false;
  bool          _bQuoteOpen = false;

  m_sData.clear();
  if ( row.empty() == false  )
    row.clear();

  if ( get_header().size() != 0 )
    row.reserve(get_header().size());

  do
  {
    if ( m_recvCacheCursor == m_recvCachedBytes )
    {
      m_recvCachedBytes = m_recvCache.size();
      csv_result _result = m_ptrDevice->recv( m_recvCache.data(), m_recvCachedBytes );
      if ( (_result != csv_result::_ok) && (m_recvCachedBytes==0) ) {
        _retVal = _result;
        
        if (_result != csv_result::_eof) {
          if (m_ptrEvents != nullptr) {
            m_ptrEvents->onError( _result );
          }
        }

        break;
      }
      m_recvCacheCursor = 0; 
    }

    _ch = static_cast<char>(m_recvCache[m_recvCacheCursor++]);

    // disabled by default, but different scientific data sources contains  
    // comment starting with '#' character
    if ( allow_comments() == true ) [[unlikely]]
    {
      if (_bSkipLine == true)
      {
        // Skip all until eol
        if (_ch == get_eol()) {
          _bSkipLine = false;
        }

        continue;
      }
      
      // A comment cannot be beween fields
      if ( (_ch == get_comment()) &&  (m_sData.empty() == true)) { 
        _bSkipLine = true; 
        continue;
      }
    }

    if ( (_ch == get_delimeter()) && (_bQuoteOpen == false)) {
      _bAddField = true;
    } else if ( _ch == get_eol() ) {
      _bAddField = true;
      _bEoL = true;
    } else {

      if (_ch == get_quote())
      {
        if ( (m_sData.empty() == true) && (_bQuoteOpen == false) )
        { _bQuoteOpen = true; }
        else if ( (m_sData.empty() == false) && (_bQuoteOpen == true) )
        { _bQuoteOpen = false; }        
      }

      if ( skip_whitespaces() && (_bQuoteOpen == false) )
      {
        // Note using memchr() instead of get_whitespaces().find_first_of(_ch) != std::string::npos )
        // with a file of 6.8 GB benchmarks show that .find_first_of(_ch) slow down overall result for ~2 seconds.
        // Reference file used for benckmarks can be found at the following address https://www.kaggle.com/devendra416/ddos-datasets 
        // at 17 May 2022 its size is 6.8 GB.
        if ( memchr( get_whitespaces().c_str(), _ch, get_whitespaces().length() ) == nullptr )
        {
          m_sData.push_back(_ch);
        }
      }
      else
      {
        m_sData.push_back(_ch);
      } 
    } // end else

    if ( _bAddField == true )
    {
      const char* pFirst = nullptr;
      const char* pLast  = nullptr;
      size_t      length = m_sData.length();

      if (m_sData.empty()==false)
      {
        pFirst = m_sData.data();
        pLast  = &pFirst[m_sData.length()-1];

        trim_all( pFirst, pLast, length );
       
        _bQuoted = is_quoted ( pFirst, pLast, length );
      }

      // A temporary object csv_field will be instantiated with
      // m_sData that is subject to copy constructor and then
      // temporary object will be moved inside the container.
      row.emplace_back( csv_field( csv_data<char,size_t>(pFirst,length), _bQuoted ) );
      
      m_sData.clear();
      _bQuoted = _bAddField = _bQuoteOpen = false;
    }

  } while (_bEoL==false);

  /////////////////////////////////////////////////////////
  // Intended to manage last data source row where line
  // terminator (EOL) could be missed then we just got EoF.
  if ( m_sData.empty() == false )
  {
    const char* pFirst = nullptr;
    const char* pLast  = nullptr;
    size_t      length = m_sData.length();

    if (m_sData.empty()==false)
    {
      pFirst = m_sData.data();
      pLast  = &pFirst[m_sData.length()-1];

      trim_all( pFirst, pLast, length );
      
      _bQuoted = is_quoted ( pFirst, pLast, length );
    }

    // A temporary object csv_field will be instantiated with
    // m_sData that is subject to copy constructor and then
    // temporary object will be moved inside the container.
    row.emplace_back( csv_field( csv_data<char,size_t>(pFirst,length), _bQuoted ) );    
    
    m_sData.clear();  
  }

  return _retVal;
}

void csv_parser::trim_all( const char* & pFirst, const char* & pLast, size_t& length ) const noexcept
{
  if ( trim_all() == true )
  {
    while ( *pFirst == ' ' ) { ++pFirst; --length; }

    if ( length > 0 ) {
      while ( *pLast  == ' ' ) { --pLast;  --length; } 
    } else {
      pLast = pFirst;
    }
  }
}

bool csv_parser::is_quoted( const char* & pFirst, const char* & pLast, size_t& length ) const noexcept
{
  // The first condition consider when we have a single character that match with get_quote()
  // in fact in such case content of the field shall be considered not quoted.
  if ( (pFirst != pLast) && (*pFirst == get_quote()) && (*pLast == get_quote()) )
  {
    pFirst++; pLast--;
    length -= 2;
    return true;
  }

  return false;
}

csv_result  csv_parser::parse( ) noexcept
{ return parse(nullptr); }

csv_result  csv_parser::parse( csv_row& row ) noexcept
{ return parse(&row); }

csv_result  csv_parser::parse( csv_row* row ) noexcept
{
  csv_result    _res   = csv_result::_ok;
  bool          _bExit = false;

  do
  {
    switch (m_eState)
    {
      case Status::eStart: 
      {
        m_nRowsCounter = 0;

        if (m_ptrEvents != nullptr) {
          m_ptrEvents->onBegin();
        }

        if ( get_header().empty() )
          m_eState = Status::eReadHeader;
        else
          m_eState = Status::eReadRows;
      }; break;
    
      case Status::eReadHeader: 
      {
        csv_row  _tmp_row;
        _res = parse_row( _tmp_row );
        if ( _res == csv_result::_ok ){
          // Initialize header with labels in the first row 
          set_header( std::move(_tmp_row) );
          //m_vHeader.init( std::move(_tmp_row) );

          // Invoke event interface  
          if (m_ptrEvents != nullptr) {
            m_ptrEvents->onHeader( m_vHeader );
          }

          m_eState = Status::eReadRows;
        }
        else if ( _res == csv_result::_eof ){
          m_eState = Status::eEnd;
        }

      }; break;

      case Status::eReadRows: 
      {
        csv_unique_ptr<csv_row> ptrRow = (row==nullptr)?std::make_unique<csv_row>():nullptr;
        csv_row&                rRow   = (row==nullptr)?(*ptrRow):(*row);

        
        _res = parse_row( rRow );
        if ( _res == csv_result::_ok  )
        {
          // Invoke event interface  
          if (m_ptrEvents != nullptr) 
          {
            if ( rRow.size() != m_vHeader.size() )
              m_ptrEvents->onError( csv_result::_row_items_error );

            ////////////////
            // providing a buffer as parameter will bypass both event and filters,
            // so we process onRow() and onFilteredRow() only if @param row is nullptr
            if ( row == nullptr )
            {
              csv_unique_ptr<csv_row> ptrRetRow = m_ptrEvents->onRow( m_vHeader, std::move(ptrRow) );

              if ( ptrRetRow != nullptr )
              {
                // Check if filters should be applied or not. 
                // Can be possible to have a filters chain for each field,
                // where the number of filters is defined at application level.                
                if ( apply_filters( *ptrRetRow, get_rows() ) )  
                {
                  m_ptrEvents->onFilteredRow( m_vHeader, std::move(ptrRetRow) );
                }
              }
            }
          }

          ++m_nRowsCounter;
          _bExit = true;
        }
        else if ( _res == csv_result::_eof ){
          m_eState = Status::eEnd;
        }
      }; break;

      case Status::eEnd: 
      {
        _res = m_ptrDevice->close();
        _bExit = true;

        if (m_ptrEvents != nullptr) {
          m_ptrEvents->onEnd();
        }        
      }; break;

      default:
        break;
    }

    /* code */
  } while ( ((_res == csv_result::_ok) || (_res == csv_result::_eof)) && (_bExit == false) );
  
  return _res;
}

} //inline namespace
} // namespace
