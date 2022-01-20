Fundamentals
===============
This section describes the fundamentals of LLRI's code design. 


Objects
----------------
LLRI objects represent information or pointers needed for rendering. Most objects are created and destroyed by the user, although some objects are acquired through the API.

Acquisition
^^^^^^^^^^^
Some objects can be acquired through API usage. For example, adapters are obtained through :func:`llri::Instance::enumerateAdapters`.

By convention, all functions prefixed with "enumerate", "get", or "query" are acquisition functions, meaning that the resources returned are created/destroyed internally, and the user does not receive any form of ownership over the returned objects. 

Creation
^^^^^^^^^
Most objects are created through create functions. Whenever the user creates an object through a create function, the user obtains the object and the ownership over the object.

By convention, all functions prefixed with "create" are creation functions. Create functions usually follow the same format, where the first parameter is a <object>_desc structure (e.g. :struct:`llri::instance_desc`), and the second parameter is a pointer-pointer to the object (e.g. :class:`llri::Instance`\*\*).

All create functions are matched with a destroy function that takes the object as its only parameter. Created objects should always be destroyed through destroy functions, and never through manual deletion.

Lifetime
^^^^^^^^
Objects in LLRI have a parent-child relationship, where any object created or acquired through another object is considered a "child" of the ("parent") object. Child objects **can not** outlive their parents, thus each child object **must** be destroyed prior to the destruction of their parent object.

If an object is acquired through a parent object, the child object pointer remains valid for the lifetime of the parent object. For example, if an :class:`llri::Adapter` is acquired through an :class:`llri::Instance`, as soon as :func:`llri::destroyInstance` is called, the :class:`llri::Adapter` pointer is no longer valid.


Extensions
-----------
The LLRI API supports various features that may not always be supported by the current environment. These features are available through extensions. LLRI commonly uses extensions to cross the gap between available features for each implementation. Per-extension support is fully **optional**.

Extensions **can** be enabled upon creation of :class:`llri::Instance` and :class:`llri::Device`. Instance extensions usually contain application-wide changes such as validation callbacks, and as such their availability tends to depend on machine configuration and/or implementation featureset. Adapter extensions are queried through :class:`llri::Adapter` and tend to depend on hardware/feature limits.

Support for extensions can be queried prior to Instance/Device creation, for :class:`llri::Instance` extensions this function is :func:`llri::queryInstanceExtensionSupport`, whereas :class:`llri::Adapter` extensions are queried through :func:`llri::Adapter::queryExtensionSupport`.

Extensions are passed as an array of extensions to instance and device description structures. See :enum:`llri::instance_extension` and :enum:`llri::adapter_extension` for more.


Validation
-----------
LLRI is an explicit API. The user has immense control over resource allocation and state, which results in low API/driver overhead. However, with this much control, it is also much easier to make mistakes. 

To aid in debugging, LLRI does parameter validation by default (disabled by defining LLRI_DISABLE_API_VALIDATION), and also comes with extensions for implementation validation (implementation message polling is disabled by defining LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING). All validation messages (LLRI validation and implementation validation) are forwarded to the validation_callback passed in :struct:`llri::instance_desc`, making it easy for engines to generate informative logs or debug runtime issues.


Multithreading
----------------
LLRI GPU commands (draw, dispatch, binding, barriers, transfers) are recorded into CommandLists, after which they can submitted to a Queue in which they are executed. In LLRI, the CommandGroup is responsible for allocating the necessary memory for CommandLists, and it is thus also responsible for encoding the commands when they're being recorded.

CommandGroups are not thread-safe within themselves, meaning that one can't record two CommandLists originating from the same CommandGroup at once, however they are separately thread-safe, meaning that a user **can** use multiple CommandGroups and record the CommandLists from those CommandGroups in separate threads simultaneously.


Multi-Adapter
-------------
Systems with multiple adapters can be utilized in two different ways in LLRI depending on their hardware configuration.

Separate Adapters
^^^^^^^^^^^^^^^^^
When a system has multiple Adapters that do not share the same feature set, they **may** be used simultaneously through LLRI. Such adapters do not have an intrinsic connection, so they're listed separately in the adapter vector retrieved from :func:`llri::Instance::enumerateAdapters()`.

To use separate Adapters simultaneously, a :class:`llri::Device` **may** be created for each one of them, after which LLRI usage continues as normal. Objects created for devices **may not** be used interchangeably between devices unless if specified so explicitly.

Linked Adapters
^^^^^^^^^^^^^^^^^
Adapters with very similar feature sets **may** support being physically linked together (Nvidia SLI, AMD Crossfire). If this is enabled, they will show up as a single adapter in :func:`llri::Instance::enumerateAdapters()` and Adapter::getNodeCount() will return more than 1. Various objects in the API will have a nodeMask value in their creation or usage. In a single-gpu setup the nodeMask can simply be set to 0 or 1 (equivalent), but with linked gpus, one might set nodeMask to (1 << index) depending on which GPU an operation should occur.