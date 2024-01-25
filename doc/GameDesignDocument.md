# GameDesignDocument

Boat racing game with torpedoes. local pvp, hitting obstacles and going off course slows you down. Torpedo hits make other players spin and slow down. Races take a few laps.

Track has collectible speed boosts that appear (semi)randomly around track. 

### Theme

While map and static objects are in 2D, ships and interactable objects are in 3D.
3D desings are low poly, but not cartoonish. Inspiration is taken from real historical vessels.
Most desings are derived from world war era ships, with one age of sail era privateer design as an easter egg.
Races take place in different areas, for example ocean, lakes and rivers. Most races take place on circuit.
Each level should have some unique characteristics.


### Technical aspects

Game is created with custom game engine. Engine is written in C++ and uses entity component system.
Since engine does not have editor, everything is made in code.
Levels should follow 16:9 aspect ratio, so every player stays visible on screen.
Exeption for levels where racetrack is linear A to B (river map).

In menu players should be able to join game with pressing a button.
Each player needs to select their ship, after ship selection player 1 selects racetrack.
For desktop version main menu should be implemented, with "start game", "options" and "quit".
"Options" should include volume controls, and possibly window size and fullscreen toggle.

Custom controller for arcade cabinet will be created. Controller has throttle lever, ship's wheel, firing and boost button.
Steering and throttle is measured with potentiometer, and turned into input with arduino.

- single screen level desing 

- custom arcade controls

- custom game engine

- Menu system


### GamePlay

Controls are meant to be as simple and intuitive as possible. Throttle and steering, fire and boost buttons.
For arcade controller, throttle and steering are done with lever and wheel, boost and fire have dedicated buttons.
Regular controllers use left and right triggers for forward and reverse, left thumbstick is used for turning left and right.
Face buttons are used for firing and boosts(A B on xbox controller).

Racetracks are narrow, but players can go "offroad" to open waters. Going offroad is penalized with bigger drag coefficient.
To finish a race, players needs to go through every checkpoint in order and cross the finish line.
Around the level are solid obstacles, fitting for the theme of a level.
There are ramps around the level that work as shortcuts. These ramps are destructible, and respawn after few seconds.
Periodically a collectable boost package appears in the racetrack. Players need to drive over it to collect. Boost disappears once collected.
Boost is activated with button press and is single use.

There are several different boats with distinct characteristics. Vessels have different shapes and weapons.
Designs vary from destroyer escorts to submarines, and as an easter egg, age of sail frigate.
Different weapon types include torpedos, autocannons, hedgehogs and broadside volleys.
Torpedoes are fast moving projectiles with long reload. Players hit with torpedo stop and spin around.
Autocannons are rapidfire weapons, with short cooldown between clips. Hitting players will slow them down.
Hedgehog is primarily anti-submarine weapon, but in this case direct hits damage surface vessels as well. 


- simple controls, acceleration, brake, steering, boost, fire 

- Main track is narrow, going off road is an option for players 

- Several boats with distinctive driving characteristics (torpedoboat, submarine, pirate ship)

- weapon variety: torpedos, cannons, hedgehogs, side volleys

- Going offroad boat slows down 

- collectibles, spawning randomly to track
    - boost
    - mines(?)

- collectibles are button activated

- Several different game modes

- Destroyable jump ramps around the level works as shortcut

- destroyable and hostile obstacles


## Core Pillars

- 2.5D asymmetric retro racing 

- local pvp, arcade cabinet/controller

- Chaotic racing 

- Party game 

