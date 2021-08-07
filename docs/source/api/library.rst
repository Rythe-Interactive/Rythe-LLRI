LLRI API
========

LLRI is designed with modern graphics APIs such as Vulkan and DirectX 12 in mind, thus multi-threading and multi-gpu both play an important role in the API's design. To accommodate for some of these things, LLRI is split up into two major levels:

- The :class:`llri::Instance` level is responsible for application-wide features such as validation callbacks, debug extensions, etc. Instance provides :func:`llri::Instance::enumerateAdapters` which enables the user to query a list of :class:`llri::Adapter` and select one of them to create a :class:`llri::Device`. LLRI **may** report multiple adapters, in which case the user **may** create multiple devices - one for each Adapter.
- The :class:`llri::Device` level is where the majority of the application's work takes place. Operations within the Device level often map loosely to allocations or commands sent to an Adapter.

The class diagram below can help give a better oversight of LLRI's objects and their dependencies:

.. image:: ../images/llri-diagram.svg

.. include:: class_view_hierarchy.rst

.. include:: unabridged_api.rst

