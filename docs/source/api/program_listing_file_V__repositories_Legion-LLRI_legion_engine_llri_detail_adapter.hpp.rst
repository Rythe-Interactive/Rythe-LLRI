
.. _program_listing_file_V__repositories_Legion-LLRI_legion_engine_llri_detail_adapter.hpp:

Program Listing for File adapter.hpp
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_V__repositories_Legion-LLRI_legion_engine_llri_detail_adapter.hpp>` (``V:\repositories\Legion-LLRI\legion\engine\llri\detail\adapter.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   //detail includes should be kept to a minimum but
   //are allowed as long as dependencies are upwards (e.g. adapter may include instance but not vice versa)
   #include <llri/detail/instance.hpp>
   
   namespace legion::graphics::llri
   {
       enum struct adapter_extension_type;
   
       enum struct adapter_type
       {
           Other,
           Integrated,
           Discrete,
           Virtual,
           MaxEnum = Virtual
       };
   
       constexpr const char* to_string(const adapter_type& type);
   
       struct adapter_info
       {
           uint32_t vendorId;
           uint32_t adapterId;
           std::string adapterName;
           adapter_type adapterType;
       };
   
       struct adapter_features
       {
   
       };
   
       class Adapter
       {
           friend Instance;
           friend result detail::impl_createInstance(const instance_desc&, Instance**, const bool&);
           friend void detail::impl_destroyInstance(Instance*);
   
       public:
           result queryInfo(adapter_info* info) const;
   
           result queryFeatures(adapter_features* features) const;
   
           result queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
       private:
           //Force private constructor/deconstructor so that only instance can manage lifetime
           Adapter() = default;
           ~Adapter() = default;
   
           void* m_ptr = nullptr;
           validation_callback_desc m_validationCallback;
           void* m_validationCallbackMessenger = nullptr;
   
           result impl_queryInfo(adapter_info* info) const;
           result impl_queryFeatures(adapter_features* features) const;
           result impl_queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
       };
   }
