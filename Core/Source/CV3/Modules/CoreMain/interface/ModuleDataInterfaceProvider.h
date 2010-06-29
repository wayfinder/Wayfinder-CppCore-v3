#ifndef MODULE_DATA_INTERFACE_PROVIDER_H
#define MODULE_DATA_INTERFACE_PROVIDER_H

// Forward declarations
class LocationProvider;

/**
 * This interface should be inherited by the instance in
 * CV3 that contains all the different modules. By offering
 * get functions for the different module specific provider interfaces
 * each module can easily get the provider interface it needs.
 */
class ModuleDataInterfaceProvider {
public:

   /** 
    * Getter that returns a pointer to the LocationProvider.
    * In CV3 this would normaly be LocationModule. LocationModule
    * implements LocationProvider hence modules that calles
    * this function can get access to the functions for the
    * LocationProvider interface.
    *
    * @return A pointer to the LocationProvider in CV3. Note that
    *         this could be NULL if no LocationProvider has been
    *         inialized.
    */
   virtual LocationProvider* getLocationProvider() const = 0;
};

#endif // MODULE_DATA_INTERFACE_PROVIDER_H
