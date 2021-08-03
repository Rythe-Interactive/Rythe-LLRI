
.. _program_listing_file_V__repositories_Legion-LLRI_legion_engine_llri_detail_adapter_extensions.hpp:

Program Listing for File adapter_extensions.hpp
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_V__repositories_Legion-LLRI_legion_engine_llri_detail_adapter_extensions.hpp>` (``V:\repositories\Legion-LLRI\legion\engine\llri\detail\adapter_extensions.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   namespace legion::graphics::llri
   {
       enum struct adapter_extension_type
       {
           MaxEnum = 0
       };
   
       constexpr const char* to_string(const adapter_extension_type& type)
       {
           switch (type)
           {
               default:
                   break;
           }
   
           return "Invalid adapter_extension_type value";
       }
   
       struct adapter_extension
       {
           adapter_extension_type type;
   
           union
           {
               //Empty until adapter extensions are added
           };
   
           adapter_extension() = default;
       };
   }
