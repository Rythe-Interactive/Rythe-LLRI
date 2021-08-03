
.. _program_listing_file_V__repositories_Legion-LLRI_legion_engine_llri_detail_instance_extensions.hpp:

Program Listing for File instance_extensions.hpp
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_V__repositories_Legion-LLRI_legion_engine_llri_detail_instance_extensions.hpp>` (``V:\repositories\Legion-LLRI\legion\engine\llri\detail\instance_extensions.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   namespace legion::graphics::llri
   {
       enum struct instance_extension_type
       {
           APIValidation,
           GPUValidation,
           MaxEnum = GPUValidation
       };
   
       constexpr const char* to_string(const instance_extension_type& result);
   
       struct api_validation_ext
       {
           bool enable : 1;
       };
   
       struct gpu_validation_ext
       {
           bool enable : 1;
       };
   
       struct instance_extension
       {
           instance_extension_type type;
           union
           {
               api_validation_ext apiValidation;
               gpu_validation_ext gpuValidation;
           };
   
           instance_extension() = default;
           instance_extension(const instance_extension_type& type, const api_validation_ext& ext) : type(type), apiValidation(ext) { }
           instance_extension(const instance_extension_type& type, const gpu_validation_ext& ext) : type(type), gpuValidation(ext) { }
       };
   
       namespace detail
       {
           [[nodiscard]] bool queryInstanceExtensionSupport(const instance_extension_type& type);
       }
   
       [[nodiscard]] bool queryInstanceExtensionSupport(const instance_extension_type& type);
   }
