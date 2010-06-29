/**
 * Copyright 2010, Wayfinder Systems AB
 */

#include "Coordinate.h"

#include <cmath>
#include <stdlib.h>
namespace wf_ngp {

Coordinate::Coordinate(const NParam* param, uint32& index)
{
   m_lat = param->incGetInt32(index);
   m_lon = param->incGetInt32(index);
}


// forward declaration of coordinate conversion function
// to make templated ctor work.
// template <typename FromCoordinate>
// Coordinate convertCoordinate(const FromCoordinate& other);

// Doesn't work - generates code as if the class is templated
// Coordinate<T>
// template <typename FromCoordinate>
// Coordinate::Coordinate(const FromCoordinate& other)
// {
// //    *this = convertCoordinate<FromCoordinate, Coordinate>(other);
// }


Coordinate::Coordinate(int32 lat, int32 lon)
   : m_lat(lat),
     m_lon(lon)
{
}


Coordinate::Coordinate()
   : m_lat(MAX_INT32),
     m_lon(MAX_INT32)
{
}


bool
Coordinate::isValid() const
{
   static const int32 MAX_ABS_LON = static_cast<int32>(M_PI * 1E8);
   static const int32 MAX_ABS_LAT = MAX_ABS_LAT / 2;
   return (abs(m_lat) < MAX_ABS_LAT && abs(m_lon) < MAX_ABS_LON);
}


void
Coordinate::serialize(NParam& param) const
{
   param.addInt32(m_lat);
   param.addInt32(m_lon);
}

} // namespace
