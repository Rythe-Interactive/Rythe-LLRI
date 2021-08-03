
.. _program_listing_file_V__repositories_Legion-LLRI_legion_engine_llri_detail_instance.hpp:

Program Listing for File instance.hpp
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_V__repositories_Legion-LLRI_legion_engine_llri_detail_instance.hpp>` (``V:\repositories\Legion-LLRI\legion\engine\llri\detail\instance.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <cstdint>
   #include <vector>
   #include <map>
   
   namespace legion::graphics::llri
   {
       enum struct result;
       struct instance_extension;
       class Instance;
       class Adapter;
   
       struct device_desc;
       class Device;
   
       enum struct validation_callback_severity
       {
           Verbose,
           Info,
           Warning,
           Error,
           Corruption,
           MaxEnum = Corruption
       };
   
       constexpr const char* to_string(const validation_callback_severity& severity);
   
       enum struct validation_callback_source
       {
           Validation,
           InternalAPI,
           MaxEnum = InternalAPI
       };
   
       constexpr const char* to_string(const validation_callback_source& source);
   
       using validation_callback = void(
           const validation_callback_severity& severity,
           const validation_callback_source& source,
           const char* message,
           void* userData
           );
   
       struct validation_callback_desc
       {
           validation_callback* callback;
           void* userData;
   
           void operator ()(const validation_callback_severity& severity, const validation_callback_source& source, const char* message) const { callback(severity, source, message, userData); }
       };
   
       struct instance_desc
       {
           uint32_t numExtensions;
           instance_extension* extensions;
           const char* applicationName;
           validation_callback_desc callbackDesc;
       };
   
       namespace detail
       {
           result impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling);
           void impl_destroyInstance(Instance* instance);
   
           using messenger_type = void;
           void impl_pollAPIMessages(const validation_callback_desc& validation, messenger_type* messenger);
       }
   
       result createInstance(const instance_desc& desc, Instance** instance);
   
       void destroyInstance(Instance* instance);
   
       class Instance
       {
           friend result detail::impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling);
           friend void detail::impl_destroyInstance(Instance* instance);
   
           friend result llri::createInstance(const instance_desc& desc, Instance** instance);
           friend void llri::destroyInstance(Instance* instance);
   
       public:
           result enumerateAdapters(std::vector<Adapter*>* adapters);
   
           result createDevice(const device_desc& desc, Device** device) const;
   
           void destroyDevice(Device* device) const;
   
       private:
           //Force private constructor/deconstructor so that only create/destroy can manage lifetime
           Instance() = default;
           ~Instance() = default;
   
           void* m_ptr = nullptr;
           void* m_debugAPI = nullptr;
           void* m_debugGPU = nullptr;
   
           validation_callback_desc m_validationCallback;
           bool m_shouldConstructValidationCallbackMessenger;
           void* m_validationCallbackMessenger = nullptr; //Allows API to store their callback messenger if needed
   
           std::map<void*, Adapter*> m_cachedAdapters;
   
           result impl_enumerateAdapters(std::vector<Adapter*>* adapters);
           result impl_createDevice(const device_desc& desc, Device** device) const;
           void impl_destroyDevice(Device* device) const;
       };
   }
