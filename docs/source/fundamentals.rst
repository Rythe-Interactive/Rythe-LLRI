Fundamentals
===============
This section describes the fundamentals of LLRI's code design. 

Objects
----------------
LLRI objects represent information or pointers needed for rendering. Most objects are created and destroyed by the user, although some objects are acquired through the API.

Acquisition
^^^^^^^^^^^
Some objects can be acquired through API usage. For example, adapters are obtained through :func:`legion::graphics::llri::Instance::enumerateAdapters`.

By convention, all functions prefixed with "enumerate" are acquisition functions, meaning that the resources returned are created/destroyed internally, and the user does not receive any form of ownership over the returned objects. 

Creation
^^^^^^^^^
Most objects are created through create functions. Whenever the user creates an object through a create function, the user obtains the object and the ownership over the object.

By convention, all functions prefixed with "create" are creation functions. Create functions all follow the same format, where the first parameter is a <object>_desc structure (e.g. :struct:`legion::graphics::llri::instance_desc`), and the second parameter is a pointer-pointer to the object (e.g. :class:`legion::graphics::llri::Instance` **).

Lifetime
^^^^^^^^
Objects in LLRI have a parent-child relationship, where any object created or acquired through another object is considered a "child" of the ("parent") object. Child objects **can not** outlive their parents, thus each child object **must** be destroyed prior to the destruction of their parent object.

If an object is acquired through a parent object, the child object pointer remains valid for the lifetime of the parent object. For example, if an :class:`legion::graphics::llri::Adapter` is acquired through an :class:`legion::graphics::llri::Instance`, as soon as :func:`legion::graphics::llri::destroyInstance` is called, the :class:`legion::graphics::llri::Adapter` pointer is no longer valid.


Extensions
-----------
The LLRI API supports various features that may not always be supported by the current environment. These features are available through extensions. LLRI uses extensions extensively to cross the gap between available features for each implementation. Per-extension support is fully **optional**.

Extensions are applied on an :class:`legion::graphics::llri::Instance` and :class:`legion::graphics::llri::Device` level. When they apply to :class:`legion::graphics::llri::Instance`, they usually dependend on machine configuration or implementation featureset, :class:`legion::graphics::llri::Device` **can** be created with :class:`legion::graphics::llri::Adapter` extensions which tend to depend on hardware/feature limits.

Support for extensions is queried prior to Instance/Device creation, for :class:`legion::graphics::llri::Instance` extensions this function is :func:`legion::graphics::llri::queryInstanceExtensionSupport`, whereas :class:`legion::graphics::llri::Adapter` extensions are queried through :func:`legion::graphics::llri::Adapter::queryExtensionSupport`.

Validation
-----------

Threading
----------------