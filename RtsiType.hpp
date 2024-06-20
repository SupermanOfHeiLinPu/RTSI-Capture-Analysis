#ifndef __RTSI_TYPE_HPP__
#define __RTSI_TYPE_HPP__

#include <array>
#include <boost/variant.hpp>

using vector3d_t = std::array<double, 3>;
using vector6d_t = std::array<double, 6>;
using vector6int32_t = std::array<int32_t, 6>;
using vector6uint32_t = std::array<uint32_t, 6>;
using RtsiTypeVariant = boost::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, int32_t, double, vector3d_t, vector6d_t, vector6int32_t, vector6uint32_t>;



#endif
