# TODO

## Math
- Change Vector3s to use doubles
- Add linear extrapolation and interpolation
- Add Matrix classes
- Add all functions and overloads to Vector2, 3, and 4
- Maybe add quaternions

## Transform
- Make a robust cache staling system at least for debug mode, maybe just a detection and warning system
- Add a local and global positions, or at least functions to get them
- Automatically update children
- Add a pivot point variable

## ECS
- Remake ECS without classes
- Add a copy entity function
- Add warning and/or asserts to possible issues
    - Trying to use an invalid Entity
    - Trying to use an unregistered component
    - Trying to access a non existent component
    - Trying to delete a non existent component
- Fix existing bugs
    - destroy entity
    - destroy component
    - ID 0 should never be used

## Rendering
- Make a unified render system
- Fix all the transparency problems
- Maybe add the Tilemap to ECS and make it better
- Sprites should automatically scale with texture size
- Add a way to set a models texture after loading
- Fix the non wireframe primitives
- Add possibility of multiple lights
- Add split screen support

## Collision
- Hide visualization in release mode
- Fix failed intersect of exact same colliders
- Add some sort of navmesh

## Physics
- Add raycasting
- Add angular momentum
- Add restraints
- Add joints

## Audio
- Simplify sound loading
- Integrate with ECS

## Camera
- Refactor to use current engine features
    - Add setters
    - Change to engine Vector3
- Add a debug camera mode which works like noclip with mouse and wasdqe

## New features
- Add pathfinding 
- Add inverse and forward kinematics