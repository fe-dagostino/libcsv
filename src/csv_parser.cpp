#include "csv_parser.h"
#include <cstring>

namespace csv {
inline namespace LIB_VERSION {

csv_parser::csv_parser( std::unique_ptr<csv_device>&& ptrDevice )
  : m_eState (Status::eStart),
    m_cSeparator(','), m_cEoL('\n'),
    m_sWhitespaces("\a\b\t\v\f\r\n"),
    m_bSkipWhitespaces(true),
    m_ptrDevice( std::move(ptrDevice) ),
    m_recvCachedBytes( 0 ),
    m_recvCacheCursor( 0 )
{
}

bool  csv_parser::set_header( csv_row&& header )
{ 
  bool bRetVal = false;
  if ( m_vHeader.empty() )
  {
    m_vHeader = std::move(header); 
    bRetVal = true;
  }

  return bRetVal;
}

csv_result csv_parser::parse_row( csv_row& row ) const
{
  char          _ch{'\0'};
  csv_result    _res = csv_result::_ok;

  bool          _bEoL      = false;
  bool          _bAddField = false;
  bool          _bQuoted   = false;

  m_sData.clear();

  do
  {
    if ( m_recvCacheCursor == m_recvCachedBytes )
    {
      m_recvCachedBytes = m_recvCache.size();
      _res = m_ptrDevice->recv( m_recvCache.data(), m_recvCachedBytes );
      if ( _res != csv_result::_ok ) {
        break;
      }
      m_recvCacheCursor = 0; 
    }

    _ch = static_cast<char>(m_recvCache[m_recvCacheCursor++]);

    if ( _ch == get_delimeter() ) {
      _bAddField = true;
    } else if ( _ch == get_eol() ) {
      _bAddField = true;
      _bEoL = true;
    } else {
      if ( skip_whitespaces() )
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
    } // else

    if ( _bAddField == true )
    {
      // A temporary object csv_field will be instantiated with
      // m_sData that is subject to copy constructor and then
      // temporary object will be moved inside the container.
      row.push_back( csv_field( m_sData, _bQuoted ) );
      
      m_sData.clear();
      _bQuoted = _bAddField = false;
    }

  } while (_bEoL==false);

  /////////////////////////////////////////////////////////
  // Intended to manage last row in the data source where 
  // line terminator (EOL) or (CR) could be missed.
  if ( !m_sData.empty() )
  {
    row.push_back( csv::csv_field( m_sData, _bQuoted ) );
    m_sData.clear();  
  }

  return _res;
}

bool  csv_parser::parse( csv_row& row ) const
{
  csv_result    _res   = csv_result::_ok;
  bool          _bExit = false;
  do
  {
    switch (m_eState)
    {
      case Status::eStart: 
      {
        if ( get_header().empty() )
          m_eState = Status::eReadHeader;
        else
          m_eState = Status::eReadLine;
      }; break;
    
      case Status::eReadHeader: 
      {
        _res = parse_row( m_vHeader );
        if ( _res == csv_result::_ok ) 
          m_eState = Status::eReadLine;
        else if ( _res == csv_result::_eof )
          m_eState = Status::eEnd;
      }; break;

      case Status::eReadLine: 
      {
        _res = parse_row( row );
        if ( _res == csv_result::_ok  )  
          _bExit = true;
        else if ( _res == csv_result::_eof )
          m_eState = Status::eEnd;
      }; break;

      case Status::eEnd: 
      {
        _res = m_ptrDevice->close();
        _bExit = true;
      }; break;

      default:
        break;
    }

    /* code */
  } while ( ( _res == csv_result::_ok ) && (_bExit == false) );
  
  return (_res == csv_result::_ok);
}



} //inline namespace
} // namespace
