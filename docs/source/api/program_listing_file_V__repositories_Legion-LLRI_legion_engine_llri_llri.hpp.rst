
.. _program_listing_file_V__repositories_Legion-LLRI_legion_engine_llri_llri.hpp:

Program Listing for File llri.hpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_V__repositories_Legion-LLRI_legion_engine_llri_llri.hpp>` (``V:\repositories\Legion-LLRI\legion\engine\llri\llri.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <cstdint>
   #include <map>
   #include <string>
   #include <vector>
   
   #if defined(DOXY_EXCLUDE)
   
   #define LLRI_DISABLE_VALIDATION
   
   #define LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
   #endif
   
   namespace legion::graphics::llri
   {
       enum struct result
       {
           Success = 0,
           Timeout,
           NotReady,
           ErrorUnknown,
           ErrorInvalidUsage,
           ErrorFeatureNotSupported,
           ErrorExtensionNotSupported,
           ErrorDeviceHung,
           ErrorDeviceLost,
           ErrorDeviceRemoved,
           ErrorDriverFailure,
           ErrorOutOfHostMemory,
           ErrorOutOfDeviceMemory,
           ErrorInitializationFailed,
           ErrorIncompatibleDriver,
           MaxEnum = ErrorIncompatibleDriver
       };
   
       constexpr const char* to_string(const result& result);
   }
   
   #include <llri/detail/instance.hpp>
   #include <llri/detail/instance_extensions.hpp>
   
   #include <llri/detail/adapter.hpp>
   #include <llri/detail/adapter_extensions.hpp>
   
   #include <llri/detail/device.hpp>
   
   // ReSharper disable once CppUnusedIncludeDirective
   #include <llri/detail/llri.inl>
