[![legion logo banner](https://cdn.discordapp.com/attachments/682321169541890070/767684570199359499/banner.png)](https://legion-engine.com)
[![build](https://github.com/Legion-Engine/Legion-Engine/workflows/build-action/badge.svg)](https://github.com/Legion-Engine/Legion-LLRI/actions?query=workflow%3Abuild-action)
[![analyze](https://github.com/Legion-Engine/Legion-Engine/workflows/analyze-action/badge.svg)](https://github.com/Legion-Engine/Legion-LLRI/actions?query=workflow%3Aanalyze-action)
[![docs](https://github.com/Legion-Engine/Legion-Engine/workflows/docs-action/badge.svg)](https://docs.legion-engine.com)
[![License-MIT](https://img.shields.io/github/license/Legion-Engine/Legion-Engine)](https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE)
[![Discord](https://img.shields.io/discord/682321168610623707.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/unVNRbd)
# Legion-LLRI
Legion Low Level Rendering Interface, a middle layer between the graphics APIs and the Legion rendering code. Aimed to allow Legion rendering code to remain graphics api agnostic. With api agnostic graphics code it is easier for Legion to support multiple platforms, and to remain independent from graphics APIs.

## Features (Planned)
### Supported APIs
- Vulkan
- DirectX 12 Ultimate

## Dependencies
(All libraries can already be found in the [deps](https://github.com/Legion-Engine/Legion-LLRI/tree/main/deps) folder)
* [Legion shader preprocessor (lgnspre)](https://github.com/Legion-Engine/LegionShaderPreprocess)
* [Vulkan SDK (for VK builds)](https://www.lunarg.com/vulkan-sdk/)

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Leon Brands** - *LLRI, API design, VK, DX* - [[Website](https://leonbrands.software)] [[Github](https://github.com/LeonBrands)] [[LinkedIn](https://www.linkedin.com/in/leonbrands/)]
* **Glyn Leine** - *Technical director, setup* - [[Website](https://glynleine.com)] [[Github](https://github.com/GlynLeine)] [[LinkedIn](https://www.linkedin.com/in/glyn-leine-7140a8167/)]

See also the list of [contributors](AUTHORS.md) who participated in this project.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details



