# Third-party components

SecretFile's own code is covered by the root [MIT license](LICENSE). That license does not replace the notices below.

| Component | Retained material | License / notice |
| --- | --- | --- |
| [raylib 6.0](https://github.com/raysan5/raylib/tree/6.0) | Downloaded and built from the pinned release | zlib/libpng; [notice](third_party/raylib-LICENSE.txt) |
| [raygui 4.0](https://github.com/raysan5/raygui/tree/4.0) | `include/raygui.h` | zlib/libpng; original header notice retained and [copied here](third_party/raygui-LICENSE.txt) |
| raygui Jungle palette | `resources/styles/jungle/style_jungle.rgs` | Palette from raygui styles, copyright (c) 2020–2023 raylib technologies; covered by the raygui notice. This copy is modified: text properties only, with embedded font data removed. |
| [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart) | Adapted Premake setup | zlib/libpng; original [quickstart notice](third_party/quickstart-LICENSE.txt) preserved |
| raylibExtras resource-directory helper | `include/resource_dir.h` | MIT, copyright (c) 2022 Jeffery Myers; [notice](third_party/resource_dir-LICENSE.txt) also retained in the header |
| [Premake](https://github.com/premake/premake-core) | Bundled build generators (`5.0.0-dev` as reported by the Windows binary) | BSD 3-Clause; [notice](third_party/premake-LICENSE.txt) |
| Export Compile Commands plugin | `build/ecc/` | BSD 3-Clause; [original notice](third_party/ecc-LICENSE.txt) also retained in `build/ecc/LICENSE.md` |

The pinned raylib source includes additional libraries for graphics, image/audio decoding and model loading. Their original header license blocks and the GLFW notice are collected in [raylib-external-NOTICES.txt](third_party/raylib-external-NOTICES.txt); this collection also includes optional components. The [GLAD upstream license and Khronos notices](third_party/glad-LICENSE.txt) are included separately. The downloaded source headers remain the authoritative source for each component's notice.

The application uses raylib's built-in default font. Roboto, the external Pixel Intv font, font-containing compiled styles, unused themes and the old sample artwork are no longer distributed in the current source/runtime package.

Windows application packages include the root license and these notices. Premake and the compile-commands plugin are build tools and are not included in the application ZIP.
