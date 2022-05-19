
#include "csv_device.h"

namespace csv {
inline namespace LIB_VERSION {

csv_device::csv_device( const std::string& sDeviceName, std::unique_ptr<csv_device_options> ptrOptions, std::unique_ptr<csv_device_events> ptrEvents )
  : m_sDeviceName(sDeviceName), m_ptrOptions(std::move(ptrOptions)), m_ptrEvents(std::move(ptrEvents)),
  m_devStats( { 0, 0, 0 } )
{}


csv_device::~csv_device()
{}

} //inline namespace
} // namespace
