Fundamentals
===============

Extensions
-----------
The LLRI API supports various features that may not always be supported by the current environment. These features are available through extensions. LLRI uses extensions extensively to cross the gap between available features for each implementation. Per-extension support is fully **optional**.

Extensions are applied on an Instance and Device level. When they apply to Instance, they usually dependend on machine configuration or implementation featureset, Devices on the other hand **can** be created with Adapter extensions which tend to depend on hardware/feature limits.

Support for extensions is queried prior to Instance/Device creation, for Instance extensions this function is :func:`legion::graphics::llri::queryInstanceExtensionSupport`, whereas Adapter extensions are queried through :func:`legion::graphics::llri::Adapter::queryExtensionSupport`.

Validation
-----------


Objects
----------------

Creation
^^^^^^^^^^^^^^^^^^^^

Lifetime
^^^^^^^^^


Threading
----------------