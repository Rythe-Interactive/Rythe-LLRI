[![legion logo banner](https://cdn.discordapp.com/attachments/682321169541890070/767684570199359499/banner.png)](https://legion-engine.com)
[![License-MIT](https://img.shields.io/github/license/Legion-Engine/Legion-Engine)](https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE)
[![Discord](https://img.shields.io/discord/682321168610623707.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/unVNRbd)

# Legion-LLRI
Legion Low Level Rendering Interface, a middle layer between the graphics APIs and the Legion rendering code. Designed to allow Legion rendering code to remain graphics API agnostic with minimal overhead. With API agnostic graphics code it is easier for Legion to support multiple platforms, and to remain independent from graphics APIs.

## Supported APIs
- Vulkan
- DirectX 12

## Dependencies
LLRI ships all of its build dependencies in the [deps](https://github.com/Legion-Engine/Legion-LLRI/tree/main/deps) folder, which requires no additional configuration.

After building, you can use LLRI with nothing more than the headers in include/, a library file of choice from lib/, and the dlls in deps/dll/. 

Additional features may require system configuration; to enable debugging extensions on DirectX builds, install "Graphics Tools" in Windows' optional features. To enable validation layers on Vulkan builds, install the Vulkan SDK from the LunarG website.

## Contributing
Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors
* **Leon Brands** - *LLRI, API design, VK, DX, Samples, Unit tests* - [[Website](https://leonbrands.software)] [[Github](https://github.com/LeonBrands)] [[LinkedIn](https://www.linkedin.com/in/leonbrands/)]
* **Glyn Leine** - *Technical director, setup* - [[Website](https://glynleine.com)] [[Github](https://github.com/GlynLeine)] [[LinkedIn](https://www.linkedin.com/in/glyn-leine-7140a8167/)]

See also the list of [contributors](AUTHORS.md) who participated in this project.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
