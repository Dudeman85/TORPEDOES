# Tiled Reference
Engine uses Tiled 2D's map editor program's tileset's and tilemap's to create a 2D map from **tmx** and **tsx** file formats which are based on xml.

For us to use tilemaps and tilesets on our engine we are using Matt Marchant's **tmxlite** library to read and edit our files in engine.

Map needs to be **Orthogonal, Base64(zlib copressed)**

## Map loading and drawing

All the functions needed to draw a map on the screen are located inside Tilemap.h. No other includes are needed in the main souurce file.

`#include <engine/Tilemap.h>`

Inside the **main** function we need to create Tilemap object which gets camera objects address as variable.
After that we can use tilemaps `loadMap()` function to draw tilemap on the screen and set tilemaps position on the screen.

```cpp
Tilemap map(&cam);
map.loadMap("address where tilemap is located");
map.position = glm::vec3(0.0f, 0.0f, 0.0f);
```

## Tilemap collider

You can specify a tilemap layer to be used as a collision layer by naming it "collider".
The tilemap collision works on a per tile basis meaning the collider is always the full size of a tile. Any tile in the collision layer will act as a collider.

To add a tilemap collision layer:
```cpp
engine.physicsSystem->SetTilemap(&map);
```

Every collision with the tilemap will be logged in the Entity's BoxCollider. The type will be Collision::Type::tilemap or Collision::Type::tilemapTrigger, and tileID will be the id of the collided tile as defined by Tiled, b will be undefined.

You can add a custom rigidbody for each tile ID, for example to make ice physics
```cpp
//Add higher friction to tile 12
engine.physicsSystem->SetTileProperty(12, Rigidbody{ .friction = 0.25 })
```