
#ifndef CSV_DEVICE_H
#define CSV_DEVICE_H

#include "csv_common.h"

namespace csv {
inline namespace LIB_VERSION {

class csv_device;

enum class csv_result {
  _ok                  = 0x0000,

  _closed          = 0x1002,
  _no_devs         = 0x0003,
  _access          = 0x0004,
  _cfg_error       = 0x0005,
  _no_mem          = 0x0006,
  _eof             = 0x0007,

  _tx_error        = 0x0008,
  _rx_error        = 0x0009,
  _tx_timedout     = 0x000A,
  _rx_timedout     = 0x000B,
  _conn_timeout    = 0x000C,

  _not_implemented,

  _wrong_protocol      = 0x2001,
  _missing_soh,
  _wrong_soh,
  _wrong_crc,
  _empty_sentence

};

struct csv_dev_stats {
  csv_uint_t   rx;
  csv_uint_t   tx;
  csv_uint_t   errors;
};

class csv_device_events
{
protected:
  /***/
  csv_device_events()
  {}
public:
  /***/
  virtual ~csv_device_events() = 0;

  /***/
  virtual void onOpened( const csv_device* pDevice ) = 0;
  /***/
  virtual void onClosed( const csv_device* pDevice ) = 0;
  /***/
  virtual void onError ( const csv_device* pDevice, csv_result result ) = 0;

};

class csv_device_options 
{
protected:
  /***/
  csv_device_options()
  {}
public:
  /***/
  virtual ~csv_device_options()
  {}

};

class csv_device
{
protected:
  /***/
  csv_device( const std::string& sDeviceName, std::unique_ptr<csv_device_options> ptrOptions, std::unique_ptr<csv_device_events> ptrEvents = nullptr );

public:
  /***/
  virtual ~csv_device();

  /***/
  constexpr const std::unique_ptr<csv_device_options>& get_options() const
  { return m_ptrOptions; }

  /***/
  virtual const std::string& getDeviceName() const
  { return m_sDeviceName; }

  /***/
  virtual void       get_stats( csv_dev_stats& stats ) const
  { stats = m_devStats; }


  /***/
  virtual csv_result open() = 0;
  /***/
  virtual csv_result send( const std::byte* pBuffer, csv_uint_t iBufferLen ) = 0;
  /***/
  virtual csv_result recv( std::byte* pBuffer, csv_uint_t& iBufferLen ) = 0;
  /***/
  virtual csv_result close() = 0;
  /***/
  virtual csv_result flush()
  { return csv_result::_not_implemented; }

  /**
   * \return _res_ok                Device is valid.
   */
  virtual csv_result is_valid() const = 0;

protected:
  std::unique_ptr<csv_device_options>    m_ptrOptions;
  std::unique_ptr<csv_device_events>     m_ptrEvents;

  /* stats */
  csv_dev_stats                          m_devStats;

private:
  const std::string                      m_sDeviceName;

};

} //inline namespace
} // namespace

#endif // CSV_DEVICE_H
