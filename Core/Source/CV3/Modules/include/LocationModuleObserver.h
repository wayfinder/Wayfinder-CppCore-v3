/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LOCATION_MODULE_OBSERVER_H
#define LOCATION_MODULE_OBSERVER_H

/**
 * This interface should be implemented by modules
 * that would like to register themself as a LocationModuleObserver
 * to the LocationModule. By doing this, the module will get
 * signals once new positions has arrived to the LocationModule.
 *
 * To add a LocationModuleObserver to the LocationModule:
 * 1. Implement this interface (inherit from this interface and
 *    ovlerload the pure virtual functions)
 * 2. Call the ModuleObserverInterfaceProvider::getLocationModuleObserverInterface
 *    to get an instance to the correct ModuleObserverInterface.
 * 3. Call ModuleObserverInterface::registerObserver(this) to get registered
 *    as an observer to the LocationModule.
 */
class LocationModuleObserver {
public:

   /** 
    * Virtual destructor with empty implementation to avoid
    * link error.
    */
   virtual ~LocationModuleObserver() {}

   /** 
    * This function will get called when the location module has gotten
    * a new position by pal.
    *
    * TODO: add parameter like Position object or similar.
    */
   virtual void signalNewPosition() = 0;
};

#endif // LOCATION_MODULE_OBSERVER_H
