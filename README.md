# Obj-PicoCAD2-Import

# Build
This only works on linux at the moment. All you need is gcc and gnu make.
```Shell
make
```

# Usage
```Shell
ObjPicoCAD2Import FILE > output.txt
```

# Road map
In no particular order.

**Final Transform**
Allow for transformations to be applied to the imported model.
(e.g. scale by 0.5)

**Texture importer** 
Crunch texture to 128x128 pixels and posterize.
Current texture import seems to pick a random color to be the 0th value (alpha).

**Save editor**
Edit an existing save by inserting a folder into the graph.

**gui implementation**
GUI would be nice.

# LICENSE
---
## MIT
```
Copyright (c) 2026 Max Kawula

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Third party licenses
**tinyobjloader-c** - MIT License - https://github.com/syoyo/tinyobjloader-c
**cJSON** - MIT License - https://github.com/DaveGamble/cJSON
