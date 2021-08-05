Getting Started
==================================
LLRI stands for "Low Level Rendering Interface" and refers to the LLRI API. The LLRI specification describes the expected behaviour of the LLRI API. 

Prerequisites
-------------
LLRI is built using Visual Studio 19 using the Clang++ compiler and C++17. For linux we currently don’t provide any default IDE support.

The default LLRI setup requires no dependencies or special configuration. Additional features may require system configuration; to enable debugging extensions on DirectX builds, install "Graphics Tools" in Windows' optional features. To enable validation layers on Vulkan builds, install the Vulkan SDK from the LunarG website.

Install
----------
Download the headers, library files and dlls from the latest GitHub release to get started immediately. To use the API, simply include the headers in include/, link your program to a .lib file of your choice from lib/, and copy the DLLs in dll/ into your executable folder.

Includes
^^^^^^^^^
All include files necessary for LLRI are found in include/llri/. You **must** only ever include llri.hpp, as all header files in the detail/ folder are included through llri.hpp and expect llri.hpp to be included first. 

Library files
^^^^^^^^^^^^^
The following library files are provided:

* **legion-llri-dx.lib** release build of the DirectX 12 implementation.
* **legion-llri-dx-d.lib** debug build of the DirectX 12 implementation.
* **legion-llri-vk.lib** release build of the Vulkan implementation.
* **legion-llri-vk-d.lib** debug build of the Vulkan implementation.

DLLs
^^^^^^
The dll/ folder contains all DLL files that LLRI **may** use. The list below describes which DLLs **must** be added and which DLLs **can** be included. Naturally, if a DLL comes with a matching .pdb file, this file **may** be included to provide additional debugging, but this is never required.

Mandatory
##########
============================	==========
DLL Name 						Description
============================	==========
dxcompiler.dll			   		HLSL compiler
dxil.dll				   		HLSL compiler (required for modern HLSL versions)
============================  	==========

Optional
########
============================	==========
DLL Name 						Description
============================	==========
WinPixEventRuntime.dll	   		[DirectX] Enables CommandList markers which are visible in debugging applications such as Microsoft PIX and NVIDIA's Nsight.
D3D12/D3D12Core.dll		   		[DirectX] Upgrades the local DirectX version to the latest DirectX Agility SDK which provides access to the latest DX12 features. If included, this DLL **must** be in the D3D12/ folder next to the executable.
D3D12/d3d12SDKLayers.dll   		[DirectX] Upgrades the local DirectX version to the latest DirectX Agility SDK which provides more robust debug features. If included, this DLL **must** be in the D3D12/ folder next to the executable.
============================  	==========

Samples
----------
LLRI comes with numerous code samples included. These samples are thoroughly commented and provide a great starting point to learning the API. They are found in /applications/samples/.

============================	==========
DLL Name 						Description
============================	==========
000_hello_llri			   		An introduction to instance creation in LLRI.
001_validation			   		Introduces the LLRI validation callback and the validation optimization flags that come along with it.
============================  	==========