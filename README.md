## Commits Of Dependencies

- `libraylib.a`: `4bfb72d8ff46b74ec8e5c745364406c4f153929f`
- `emsdk`: `d7b4130688bfa348392b921ab04015aa15c4f154`

## How To Build (Linux Only)
1. Clone the [emsdk](https://github.com/emscripten-core/emsdk) in `$HOME/.local/share` (optional: use `--depth=1` for a faster clone)
2. Follow the instructions to install and activate [emsdk](https://github.com/emscripten-core/emsdk)
3. Compile Raylib with [the following instructions](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5))
4. Move `libraylib.a` to the root of the project
5. Run `build.sh`
6. Run any [HTTP server](https://www.npmjs.com/package/http-server) in `/build` and visit it

### Raylib Compile Flags

The following commands were used from the `/src` directory of the Raylib project:

```bash
emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c raudio.c -Os -Wall -DPLATFORM_WEB

emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o
```

## Licenses
- Code written for this project is licensed under the 0BSD license
- Assets made for this project are licensed under the CC0 license
- Code and assets with their license attached or embedded are not relicensed
- Other code and assets are public domain or CC0

## Windows Users

Figure it out or make a PR.
