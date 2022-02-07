Getting Started
==================================
LLRI stands for "Low Level Rendering Interface" and refers to the LLRI API. The LLRI specification describes the expected behaviour of the LLRI API. 

Platforms
------------
LLRI is built with C++17, and supports the following configurations:

============================	==========	====================
Operating system				Compiler	Implementation(s)
============================	==========	====================
Windows 10 (x64)				MSVC		Vulkan / DirectX 12
Windows 10 (x64)				LLVM Clang	Vulkan / DirectX 12
MacOS 11 Big Sur				G++			Vulkan (MoltenVK)
MacOS 11 Big Sur				Clang++		Vulkan (MoltenVK)
Ubuntu 20.0.4					G++			Vulkan
Ubuntu 20.0.4					Clang++		Vulkan
============================	==========	====================

Prerequisites
-------------
LLRI uses CMake 3.16 (or higher) for project generation, and is currently actively tested on Visual Studio 19 and Xcode. 

If you're on MacOS and wish to use the Vulkan (MoltenVK) implementation, download the Vulkan SDK for MacOS from the LunarG website. Additional features may also require system configuration; to enable debugging extensions on DirectX builds, install "Graphics Tools" in Windows' optional features. To enable validation layers on Vulkan builds, install the Vulkan SDK from the LunarG website.

Build
----------
* Install Git: https://git-scm.com/downloads
* Clone the repo: `git clone https://github.com/Rythe-Interactive/Rythe-LLRI.git`
* Update the submodules: `git submodule update --init --recursive`
* Install CMake 3.16 or higher: https://cmake.org
* Generate the project: cmake -S path/to/llri -B path/to/llri -G "Visual Studio 16 2019"
	* Add -T ClangCL to use LLVM Clang in Visual Studio
	* Change the -G parameter to "Xcode", "Visual Studio 16 2019", "Unix Makefiles"

Install
----------
If you don't have a need to build LLRI yourself, you may instead download the prebuilt headers & binaries. Download the latest GitHub release to get started immediately (Note: Not available until LLRI 1.0.0). To use the API, simply include the headers in include/, link your program to a static library file of your choice from lib/, and copy the DLLs in dll/ into your executable folder.

Includes
^^^^^^^^^
All include files necessary for LLRI are found in include/llri/. You **must** only ever include llri.hpp, as all header files in the detail/ folder are included through llri.hpp and expect llri.hpp to be included first. 

Library files
^^^^^^^^^^^^^
The following library files (.lib, .a or .so) are provided:

* **llri-dx** release build of the DirectX 12 implementation.
* **llri-dx-d** debug build of the DirectX 12 implementation.
* **llri-vk** release build of the Vulkan implementation.
* **llri-vk-d** debug build of the Vulkan implementation.

DLLs
^^^^^^
The dll/ folder contains all DLL files that LLRI **may** use. The list below describes which DLLs **must** be added and which DLLs **can** be included. Naturally, if a DLL comes with a matching .pdb file, this file **may** be included to provide additional debugging, but this is never required.

Mandatory
##########
============================	==========
DLL Name 						Description
============================	==========
dxcompiler.dll					HLSL compiler
dxil.dll						HLSL compiler (required for modern HLSL versions)
============================	==========

Optional
########
============================	==========
DLL Name						Description
============================	==========
WinPixEventRuntime.dll			[DirectX] Enables CommandList markers which are visible in debugging applications such as Microsoft PIX and NVIDIA's Nsight.
D3D12/D3D12Core.dll				[DirectX] Upgrades the local DirectX version to the latest DirectX Agility SDK which provides access to the latest DX12 features. If included, this DLL **must** be in the D3D12/ folder next to the executable.
D3D12/d3d12SDKLayers.dll		[DirectX] Upgrades the local DirectX version to the latest DirectX Agility SDK which provides more robust debug features. If included, this DLL **must** be in the D3D12/ folder next to the executable.
============================	==========

Samples
----------
LLRI comes with numerous code samples included. These samples are thoroughly commented and provide a great starting point to learning the API. They are found in /applications/samples/.

============================	==========
DLL Name 						Description
============================	==========
000_hello_llri			   		An introduction to instance creation in LLRI.
001_validation			   		Introduces the LLRI validation callback and the validation optimization flags that come along with it.
002_validation_extensions		Extends the validation callback with implementation and GPU-based validation.
003_adapter_selection			Displays how to acquire Adapters and how to select a desired adapter based off its properties.
004_device						Use the selected Adapter to create a Device; An object that represents the Adapter and creates objects for it.
005_commands					Create a CommandGroup and allocate & record a CommandList using the Device object.
006_queue_submit				Learn how to submit a CommandList to a Queue, after which they get executed on the GPU.
007_swapchain					Connect LLRI to a windowing system to enable drawing to the window.
============================  	==========