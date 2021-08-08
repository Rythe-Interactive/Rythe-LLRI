Fundamentals
===============
This section describes the fundamentals of LLRI's code design. 

Objects
----------------
LLRI objects represent information or pointers needed for rendering. Most objects are created and destroyed by the user, although some objects are acquired through the API.

Acquisition
^^^^^^^^^^^
Some objects can be acquired through API usage. For example, adapters are obtained through :func:`llri::Instance::enumerateAdapters`.

By convention, all functions prefixed with "enumerate" are acquisition functions, meaning that the resources returned are created/destroyed internally, and the user does not receive any form of ownership over the returned objects. 

Creation
^^^^^^^^^
Most objects are created through create functions. Whenever the user creates an object through a create function, the user obtains the object and the ownership over the object.

By convention, all functions prefixed with "create" are creation functions. Create functions all follow the same format, where the first parameter is a <object>_desc structure (e.g. :struct:`llri::instance_desc`), and the second parameter is a pointer-pointer to the object (e.g. :class:`llri::Instance`\*\*).

All create functions are matched with a destroy function that takes the object as its only parameter. Created objects should always be destroyed through destroy functions, and never through manual deletion.

Lifetime
^^^^^^^^
Objects in LLRI have a parent-child relationship, where any object created or acquired through another object is considered a "child" of the ("parent") object. Child objects **can not** outlive their parents, thus each child object **must** be destroyed prior to the destruction of their parent object.

If an object is acquired through a parent object, the child object pointer remains valid for the lifetime of the parent object. For example, if an :class:`llri::Adapter` is acquired through an :class:`llri::Instance`, as soon as :func:`llri::destroyInstance` is called, the :class:`llri::Adapter` pointer is no longer valid.


Extensions
-----------
The LLRI API supports various features that may not always be supported by the current environment. These features are available through extensions. LLRI uses extensions extensively to cross the gap between available features for each implementation. Per-extension support is fully **optional**.

Extensions are inserted upon creation of :class:`llri::Instance` and :class:`llri::Device`. Instance extensions usually contain application-wide changes such as validation callbacks, and as such their availability tends to depend on machine configuration and/or implementation featureset. Device extensions are queried through :class:`llri::Adapter` and tend to depend on hardware/feature limits.

Support for extensions is queried prior to Instance/Device creation, for :class:`llri::Instance` extensions this function is :func:`llri::queryInstanceExtensionSupport`, whereas :class:`llri::Adapter` extensions are queried through :func:`llri::Adapter::queryExtensionSupport`.

Extensions are passed as an array of extension structures to instance and device description structures. See :struct:`llri::instance_extension` and :struct:`llri::adapter_extension` for more.

Validation
-----------
LLRI is an explicit API. The user has immense control over resource allocation and state, which results in low API/driver overhead. However, with this much control, it is also much easier to make mistakes. 

To aid in debugging, LLRI does parameter validation by default (disabled by defining LLRI_DISABLE_API_VALIDATION), and also comes with extensions for internal API validation (internal API message polling is disabled by defining LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING). All validation messages (LLRI validation and internal API validation) are forwarded to the validation_callback passed in :struct:`llri::instance_desc`, making it easy for engines to generate informative logs or debug runtime issues.

Threading
---------------- 
//TODO Write about CommandLists once they exist
