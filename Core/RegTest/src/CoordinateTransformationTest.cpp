/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CoordinateTransformationTest.h"

#define CORE_LOGGING_MODULE_NAME "COORDINATE_TRANSFORMATION_TEST"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#include "CoreDebugPrint.h"

#include "Coordinate.h" // ngp
#include "WGS84Coordinate.h"

#include <cmath>
#include <sstream>

using wf_ngp::Coordinate;
using WFAPI::WGS84Coordinate;

CoordinateTransformationTest::CoordinateTransformationTest(const char* name)
   : RegressionTest(name)
{
}

CoordinateTransformationTest::~CoordinateTransformationTest()
{
}

// internal helper
static
std::string
formatConversionError(const std::string& entity,
                      double absDifference,
                      double maxAbsDifference) {
   std::ostringstream os;
   os << "Error when converting "
      << entity
      << " abs(difference): "
      << absDifference
      << " max tolerance: "
      << maxAbsDifference;
   return os.str();
}

void CoordinateTransformationTest::startTest()
{
   const Coordinate coord;
   if (coord.isValid()) {
      setTestFailed("No-arg constructor of Coordinate must yield invalid"
                    " invalid coordinate.");
      return;
   }
   const WGS84Coordinate w0(coord);
   if (w0.isValid()) {
       setTestFailed("Conversion of invalid Coordinate should yield"
                      " invalid result.");
       return;
   }

   // doesn't compile since ctor is protected - is correct!
   //CoordinateBase cb;

   const WGS84Coordinate officeWGS(55.613131, 12.992237);
   if (! officeWGS.isValid()) {
      setTestFailed("WGS84Decimal(55.613131, 12.992237) must be valid.");
      return;
   }

   const Coordinate officeNGP(officeWGS);
   if (! officeWGS.isValid()) {
      setTestFailed("Conversion of valid WGS84Coordinate must yield"
                    " valid Coordinate.");
      return;
   }

   const WGS84Coordinate officeFromNGP(officeNGP);
   if (! officeFromNGP.isValid()) {
      setTestFailed("Conversion of valid Coordinate must yield"
                    " valid WGS84Coordinate.");
      return;
   }
   // abs() is overloaded and the int-version is picked unless you cast.
   double latDiff = fabs(officeWGS.latDeg - officeFromNGP.latDeg);
   double lonDiff = fabs(officeWGS.lonDeg - officeFromNGP.lonDeg);
   // 0.1m error
   const double MAX_ERROR = 360 * 0.1 / (2 * M_PI * 6378137.0);
   if (latDiff > MAX_ERROR) {
      setTestFailed(formatConversionError("latitude",
                                          latDiff,
                                          MAX_ERROR));                    
      return;
   }
   if (lonDiff > MAX_ERROR) {
      setTestFailed(formatConversionError("longitude",
                                          lonDiff,
                                          MAX_ERROR));                    
      return;
   }
   

   setTestPassed();
   return;
}
