/**
 * Copyright 2010, Wayfinder Systems AB
 */

#ifndef NGPCOORDINATE_H
#define NGPCOORDINATE_H

#include "NGP.h"

namespace wf_ngp {
// forward decl of class needed for templated function
class Coordinate;

// if you want to use Coordinate templated copy constructor
// you must specialize and implement this function
template <typename FromCoordinate, typename ToCoordinate>
ToCoordinate convertCoordinate(const FromCoordinate& other);


/**
 * Coordinate data.
 *
 * The coordinate type is not defined as a stand-alone entity in the
 * protocol specification. The type is implied by params 1001, 1011, 1206, ...
 */
class Coordinate {
public:

   /**
    * Constructor from NParam.
    *  
    * @param param The parameter which contains the data for the members.
    * @param index The current index for the parameter.
    */
   Coordinate(const NParam* param, uint32& index);

   /**
    * Constructs a Coordinate from the supplied coordinate of
    * some other type.
    */
   template <typename FromCoordinate>
   Coordinate(const FromCoordinate& other) {
      // doesn't work to define this in cpp-file - gcc 4.2.4 doesn't
      // generate the templated code I want but perhaps does what the
      // standard says...
      *this = convertCoordinate<FromCoordinate, Coordinate>(other);
   }

   /**
    * Constructor from NGP latitude and longitude.
    *
    * This is for internal use in ngplib only as it exposes the
    * protocol format coordinates.
    *
    * The NGP coordinate format is WGS84 radians * 1E8 which yields a
    * minimal precision of about 0.01 m at the equator.
    *
    * @param lat latitude in NGP format.
    * @param lon longitude in NGP format.
    */
   Coordinate(int32 lat, int32 lon);

   /**
    * Default constructor which yields an invalid coordinate.
    *
    * This is to make some STL-constructs like vector::resize() easier
    * but makes it less visible to the user that the new coordinates
    * are not valid. But at least they will cause errors when talking
    * to the server and not yield a blue map for (0,0) which is in
    * south Atlantic off the coast of Nigeria.
    */
   Coordinate();

   /**
    * Compare equals.
    */
   bool operator == (const Coordinate& rhs) const {
      return
         m_lat == rhs.m_lat &&
         m_lon == rhs.m_lon;
   }
   
   /**
    * Checks if the Coordinate is valid.
    *
    * Has not been tested around the extreme limits
    */
   bool isValid() const;
   
   /**
    *  Coordinate serialization function
    *  @param param The param that will be filled with data.
    */ 
   void serialize(NParam& param) const;

   /**
    * @return the latitude of the coordinate.
    */
   int32 getLat() const {
      return m_lat;
   }

   /**
    * @return the latitude of the coordinate.
    */
   int32 getLon() const {
      return m_lon;
   }

private:
   /// Latitude in NGP format.
   int32 m_lat;

   /// Longitude in NGP format.
   int32 m_lon;
};

} // namespace
#endif /* NGPCOORDINATE_H */
