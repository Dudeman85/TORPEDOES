# TextRender Reference

Engine uses Freetype library to read from **TrueType font** (**.ttf**) file character glyphs. It then renders
text on the screen by combining correct characters from string text and character glyphs together.

At the moment Scandinacian letters can't be rendered on the screen so game is in english.

## TrueType font loading and rendering

All the necessary functions are located into **Font.h** and **TextRender.h** file and file is made part of **ECS** system.

Remember to add `#include <engine/Application.h>` and also `ECS ecs` in the beginning of the **main.cpp**

Inside the `int main()` function add `engine::EngineLib engine;` to initiate Enginelib library so TextRnder component can be used.

```cpp
/*
* Font constructor which can be placed under engine::EngineLib engine
*
* - filepathname: Where the .ttf file is in asset folder
* - face_index: If the .ttf file has multiple fonts in it this indicates a starting position of the font
*				Can be left as 0 if .ttf only has one font type.
* - pixel_width: How large is the character glyph in font.
*				If left at 0 width will be automaticly adjusted to be same as height.
* - pixel_height: How tall is the character glyph in font.
*/

Font(const char* filepathname, FT_Long face_index, FT_UInt pixel_width, FT_UInt pixel_height);
```

```cpp

/*
* Function that will add the TextRenderer component into Entity Id.
*
*.font = &Font: Here we give our just created Font class.
* .text = string: What text we want to render. Need to be given in variable form.
* .offset = Vector3(): Some of the Character glyphs are slightly below horizontal x baseline so this will position characters properly.
* .scale = Vector3(): The size of the characters when rendered
* .color = vector3(): Characters color when they are rendered
*/

ecs.addComponent(Entity, TextRenderer{ .font = &Font, .text = string, .offset = Vector3(), .scale = Vector3(), .color = Vector3() });
```


