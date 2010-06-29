#ifndef MODULE_OBSERVER_INTERFACE_PROVIDER_H
#define MODULE_OBSERVER_INTERFACE_PROVIDER_H

// Forward declarations
class ModuleObserverInterface;

/**
 * This interface should be inherited by the instance in
 * CV3 that contains all the different modules. By offering
 * get functions for the different module specific interfaces
 * each module can easily get the observer interface it needs.
 */
class ModuleObserverInterfaceProvider {
public:

   /** 
    * Getter that returns a pointer to the
    * LocationModuleObserverInterface, this is really
    * the LocationModule that inherites from ModuleObserverInterface
    * and doing this, modules that inherites from
    * LocationModuleObserver can add themself as observers
    * to the location module, through the ModuleObserverInterface,
    * which they get by calling this function.
    *
    * @return A pointer to the ModuleObserverInterface, in this
    *         case the LocationModule. Be aware that this
    *         function might return NULL if no LocationModule
    *         has been initialized.
    */
   virtual ModuleObserverInterface*
   getLocationModuleObserverInterface() const = 0;
};

#endif // MODULE_OBSERVER_INTERFACE_PROVIDER_H
