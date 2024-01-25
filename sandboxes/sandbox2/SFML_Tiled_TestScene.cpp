//#include "engine/Application.h"
////Create one instance of the ecs manager
//ECS ecs;
//
//
//int main()
//{
//    setlocale(LC_ALL, "");
//
//    /*
//    ECS test
//    -----------------------------------------------
//     // New engine instance, contains basic systems
//    engine::EngineLib engine;
//     //Create a new entity
//    Entity player = ecs.newEntity();
//    // Adding sprite to player
//    ecs.addComponent(player, engine::Sprite());
//    // Adding transform to player
//    ecs.addComponent(player, engine::Transform());// Loading texture
//    // new sf texture
//    sf::Image spritesheet = engine::LoadImage("texturemap1.png");
//      //first sprite
//    sf::Texture texture = engine::CustomSlice(spritesheet, 0, 0, 64, 64);
//    // players sprite component reference
//    engine::Sprite& spriteComponent = ecs.getComponent<engine::Sprite>(player); // OpenGL broken no need to worry
//    // Sprite component texture
//    spriteComponent.texture = texture;
//    // set player position
//    engine.transformSystem->setPosition(player, 100, 60); // OpenGL broken no need to worry
//    // Rendering
//        engine.renderSystem->Update(window);
//    ----------------------------------------------------------------------------------------
//    */
//
//    // List for Colliders
//    std::vector<sf::FloatRect> colliders;
//
//    // Tiled Map class
//    tmx::Map map;
//
//    // SFML window creation
//    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
//
//    // tmx load function for Tiled assets
//    //map.load("assets/demo.tmx");
//   // map.load("assets/Tiled/maps/ColliderTestMap.tmx");
//
//    // See has the Tiled map been loaded
//    if (map.load("assets/Tiled/maps/ColliderTestMap.tmx"))
//    {
//        // Debug print
//        std::cout << "Tiled versio > " << map.getVersion().upper << "," << map.getVersion().lower << std::endl;
//
//        // Get Tiled maps address
//        const auto& layers = map.getLayers();
//
//        // Debug print
//        std::cout << "Mapilla on > " << layers.size() << " layeriä" << std::endl;
//
//        // Go through all the layers
//        for (const auto& layer : layers)
//        {
//            // Debug prints
//            std::cout << "Layeri löydetty > " << layer->getName() << std::endl;
//            std::cout << " Layerin tyyppi > " << int(layer->getType()) << std::endl;
//
//            // See if we have found object layer
//            if (layer->getType() == tmx::Layer::Type::Object)
//            {
//                // Place Tiled layer objects address to object variable
//                const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
//
//                // Debug print
//                std::cout << "Löytyi > " << objects.size() << " objektia layeristä" << std::endl;
//
//                // Go through the objects in object layer
//                for (const auto& object : objects)
//                {
//                    // Debug print
//                    std::cout << "Objekti" << object.getUID() << ", " << object.getName() << std::endl;
//
//                    // All colliders AABB
//                    tmx::FloatRect rect = object.getAABB();
//
//                    // store colliders into library
//                    colliders.push_back(sf::FloatRect(rect.left, rect.height, rect.top, rect.width));
//                }
//            }
//
//            const auto& tilesets = map.getTilesets();
//
//            for (const auto& tileset : tilesets)
//            {
//                //read out tile set properties, load textures etc...
//            }
//        }
//    } 
//
//    // Layers creation
//    MapLayer layerZero(map, 0);
//    MapLayer layerOne(map, 1);
//   // MapLayer layerTwo(map, 2);
//    // run the program as long as the window is open
//    // Game loop
//    while (window.isOpen()) {
//        // check all the window's events that were triggered since the last iteration of the loop
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            // "close requested" event: we close the window
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//        window.clear(sf::Color::Cyan);
//
//        
//        // layer drawing
//        window.draw(layerZero);
//        window.draw(layerOne);
//       // window.draw(layerTwo);
//        // 4: end the current frame (swap buffers)
//        window.display();
//    } // End - while()  
//    return 0;
//}
//
//// TOIMIVA TILED!!!!
//#include "engine/Application.h"
//
////Create one instance of the ecs manager
//ECS ecs;
//
//int main()
//{
//    // Tiled Map class
//    tmx::Map map;
//
//    // SFML window creation
//    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
//
//    // Download Tiled map from file
//    map.load("assets/Tiled/maps/CollidersTestMap.tmx");
//
//    // Load tileset Texture
//    sf::Texture tilesetTexture;
//    tilesetTexture.loadFromFile("assets/Tiled/tilesets/FD_Dungeon_Free.png");
//
//    // Finding all the layers
//    const auto& layers = map.getLayers();
//
//    // Creating Box2d-world
//    b2World world(b2Vec2(0.0f, 9.8f));
//
//    // Game loop
//   // Run the program as long as the window is open
//    while (window.isOpen())
//    {
//
//        // Check all the window's events that were triggered since the last iteration of the loop
//        sf::Event event;
//
//        while (window.pollEvent(event))
//        {
//            // "Close requested" event: we close the window
//            if (event.type == sf::Event::Closed)
//            {
//                window.close();
//            }
//        }
//
//        window.clear(sf::Color::Cyan);
//
//    // Going through all the layers
//    for (const auto& layer : layers)
//    {
//        // See if we have found object layer
//        if (layer->getType() == tmx::Layer::Type::Object)
//        {
//            // Place Tiled layer objects address to object variable
//            const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
//
//            // Go through the objects in object layer
//            for (const auto& object : objects)
//            {
//                // Check if the object is rectangle shaped
//                if (object.getShape() == tmx::Object::Shape::Rectangle)
//                {
//                    // Change the Tiled-Objects coordinats to Box2d: format
//                    sf::FloatRect rect;
//                    rect.left = object.getAABB().left;
//                    rect.top = object.getAABB().top;
//                    rect.width = object.getAABB().width;
//                    rect.height = object.getAABB().height;
//                    b2Vec2 pos(rect.left + rect.width / 2, rect.top + rect.height / 2);
//                    b2Vec2 size(rect.width / 2, rect.height / 2);
//
//                    // Creating Box2D body
//                    b2BodyDef bodyDef;
//                    bodyDef.position.Set(pos.x, pos.y);
//                    bodyDef.type = b2_staticBody;
//                    b2Body* body = world.CreateBody(&bodyDef);
//
//                    // Creating Box2D collider
//                    b2PolygonShape shape;
//                    shape.SetAsBox(size.x, size.y);
//                    b2FixtureDef fixtureDef;
//                    fixtureDef.shape = &shape;
//                    body->CreateFixture(&fixtureDef);
//
//                }
//            }
//        }
//        else if (layer->getType() == tmx::Layer::Type::Tile)
//        {
//            // Place Tiled layer address to tile variable
//            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
//
//            // Get all tiles in the layer
//            const auto& tiles = tileLayer.getTiles();
//
//            // Get the width and height of the tile layer
//            int width = tileLayer.getSize().x;
//            int height = tileLayer.getSize().y;
//
//            // Calculate tile size
//            int tileWidth = map.getTileSize().x;
//            int tileHeight = map.getTileSize().y;
//
//            // Loop through each tile in the layer
//            for (int y = 0; y < height; y++)
//            {
//                for (int x = 0; x < width; x++)
//                {
//                    // Get the tile at the current position
//                     auto tileId = tiles[y * width + x].ID;
//
//                    // Calculate the position of the tile
//                    int posX = x * tileWidth;
//                    int posY = y * tileHeight;
//
//                    // Create sprite for the tile and set its texture
//                    sf::Sprite tileSprite(tilesetTexture);
//     
//                     // Set the texture rect for the sprite
//                    int tilesetWidth = tilesetTexture.getSize().x;
//                    int tilesetColumns = tilesetWidth / tileWidth;
//                    int tilesetX = (tileId % tilesetColumns) * tileWidth;
//                    int tilesetY = (tileId / tilesetColumns) * tileHeight;
//                    tileSprite.setTextureRect(sf::IntRect(tilesetX, tilesetY, tileWidth, tileHeight));
//
//                    // Set the position for the sprite
//                    tileSprite.setPosition(posX, posY);
//
//                    // Draw the tile sprite
//                    window.draw(tileSprite);
//
//                }
//            }
//        }
//
//    }
//    
//    // End the current frame (Swap buffers)
//    window.display();
//
//    } // End - while()
//    return 0;
//}
//
//
//int main()
//{
//    return 0;
//}