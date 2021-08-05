Terminology
=================
This section aims to clarify the standards set throughout LLRI's documentation.

Names
-------
LLRI often refers to parts of the library by certain words or names:

============================	==========
Name							Meaning
============================	==========
(the) API						LLRI's public API, as described in the documentation. The API is the part of LLRI that is used directly by the user. 
Operation 						An action or command given to the LLRI API by the user. Operations do many things, such as allocate GPU memory or send draw calls.
Implementation					A buildable implementation of the LLRI API. Implementations usually integrate a graphics API (e.g. Vulkan or DirectX 12) and then map LLRI operations to the API's operations.
============================  	==========

Keywords
---------
LLRI uses keywords that have binding implications. These keywords only take binding effect whenever they are **bolded**. They usually describe rules to which an implementation of the API must adhere to. 

The keywords loosely follow the https://www.ietf.org/rfc/rfc2119.txt convention, but their exact meaning within the API/documentation is described below:

============================	==========
Keyword(s) 						Meaning
============================	==========
must, required, shall   		The definition is an absolute requirement.
must not, shall not				The definition is an absolute prohibition.
should, recommended				There may exist valid reasons in particular circumstances to ignore the definition.
should not, not recommended		There may exist valid reasons in particular circumstances when the definition is acceptable or even useful.
may, optional, optionally		The definition is truly optional. 
============================  	==========

Something important to note regarding these keywords; Whenever they're used in a context that is describing the API, they apply to the implementation's behaviour, whereas in a context that addresses developers it expresses requirements to the behaviour of the developer's application.
