Rythe-LLRI
==================================
Rythe-LLRI, or "Rythe Low Level Rendering Interface" is a rendering interface that aims to provide a graphics API agnostic approach to graphics development.

LLRI is designed as a low overhead API that helps developers write graphics API / platform agnostic code without the typical limitations of higher level abstractions.

.. warning::
    The API and its wiki pages are still under heavy development. If you find yourself stuck with a problem, do not hesitate to approach the rythe graphics team!

Overview
--------
LLRI accomplishes its low overhead graphics API agnostic code by providing a shared public API through its headers, and API specific implementations through its static library variations. Users simply use the API through the headers and statically link to whichever implementation they'd prefer to use.

A user's implementation of choice may depend on preference or platform requirements. Regardless of the user's choice, implementations of the LLRI specification are obliged to adhere to the API specification. LLRI is designed with modern graphics APIs (Vulkan, DirectX 12) in mind and tries to be as inclusive to these APIs' features as possible, providing maximum feature compatibility with as little overhead as possible. 

Pages
----------------
.. toctree::
    :maxdepth: 2

    getting-started.rst
    terminology.rst
    fundamentals.rst
    api/library.rst
