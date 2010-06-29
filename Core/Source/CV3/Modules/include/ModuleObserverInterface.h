/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MODULE_OBSERVER_INTERFACE_H
#define MODULE_OBSERVER_INTERFACE_H

/**
 * This class is the base class for all the modules that
 * offers an interface for other members of CV3 to register
 * itself to and by doing so get callbacks from the module
 * it have registered itself as an observer for.
 *
 * For example, the SearchModule might want to have updates
 * when the LocationModule gets a new position from PALLocation
 * or similar. The SearchModule then inherites from
 * LocationModuleObserver and implements the pure virtual
 * functions from that Observer interface. SearchModule then calls
 * CV3Main->getLocationModuleObserverInterface()->registerObserver(this)
 * Once the LocationModule gets a new position it will go throuh it's set
 * of listeners and call signalNewPosition (part of LocationModuleObserver).
 *
 * All modules that offers this kind of interface should inherit
 * from this class. An example of how the LocationModule does:
 * class LocationModule : private ModuleObserverInterface<LocationModuleObserver>
 * This initializes ModuleObserverInterface to contain objects of
 * LocationModuleObserver type.
 */
template<class T>
class ModuleObserverInterface {
public:

   /** 
    * Adds an observer to the set of observers. By doing this
    * the observer will get callbacks for the Observer interface
    * it has overloaded.
    * 
    * @param observer A pointer to an observer, the type of observer
    *                 will be defined through inheritence.
    */
   virtual void registerObserver(T* observer) {
      m_observers.insert(observer);
   }

   /** 
    * Deregisters an observer, removes the observer from the set
    * of observers. The observer will from that on not get any
    * callbacks from the Observer interface.
    * 
    * @param observer A pointer to the observer to deregister.
    */
   virtual void deregisterObserver(T* observer) {
      m_observers.erase(observer);
   }
   
protected:
   
   /// Typedef for a set of observers.
   typedef std::set<T*> ObserverContainer;

   /// Set of observers
   ObserverContainer m_observers;
};

#endif // MODULE_OBSERVER_INTERFACE_H
