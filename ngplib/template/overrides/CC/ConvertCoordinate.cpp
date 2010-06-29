#include "WGS84Coordinate.h"
#include "Coordinate.h"

#include <cmath>

using wf_ngp::Coordinate;
using WFAPI::WGS84Coordinate;

using wf_ngp::int32;


// see navserver_prot_2_10.pdf: p5
// see cppcore:Targets/MapLib/Export/internal/include/MC2Coordinate.h
// see cppcore:Targets/MapLib/Shared/src/MC2Coordinate.cpp
namespace {

int32
wgs84DecToNgp(double wgs84Dec) {
   static const double DEGREE_TO_NAV2_COORD_FACTOR =
      /*
       * convert to degrees and multiply. But change the order so
       * we don't massively upscale the truncation error.
       * 
       * No decimals in protocol so
       * max error = 1E-8 radians ~ 6E-7 degrees ~ 1 cm at equator
       * this is the same accuracy order of magnitude as
       * a MC2Coordinate.
       */ 
      (M_PI * 1E8) / 180;
         
   return static_cast<int32>(wgs84Dec * DEGREE_TO_NAV2_COORD_FACTOR);
}

// reverse of wgs84DecToNgp()
double
ngpToWgs84Dec(int32 ngp) {
   static const double NAV2_COORD_TO_DEGREE_FACTOR =
      180 / (M_PI * 1E8);

   return ngp * NAV2_COORD_TO_DEGREE_FACTOR;
}
} // anonymous namespace


namespace WFAPI {
template <>
WGS84Coordinate convertCoordinate<Coordinate, WGS84Coordinate> (
   const Coordinate& other)
{
   return WGS84Coordinate(ngpToWgs84Dec(other.getLat()),
                          ngpToWgs84Dec(other.getLon()));
}
} // namespace WFAPI


namespace wf_ngp {
template <>
Coordinate convertCoordinate<WGS84Coordinate, Coordinate> (
   const WGS84Coordinate& other)
{
   return Coordinate(wgs84DecToNgp(other.latDeg),
                     wgs84DecToNgp(other.lonDeg));
}
}
