#pragma once

// Set the required components for this System
#define ECS_REQUIRED_COMPONENTS(CLASSNAME, ...) struct CLASSNAME; \
inline bool CLASSNAME##Signatured = \
    (\
        SystemManager::getInstance().setRequiredComponents<CLASSNAME>({__VA_ARGS__}), \
        true \
    );

// Register component
#define ECS_REGISTER_COMPONENT(CLASSNAME) struct CLASSNAME; \
    inline bool CLASSNAME##Registered = \
    (\
        (ComponentManager::getInstance().autoRegister<CLASSNAME>()), \
        true \
    );

// Create a registered component
#define ECS_COMPONENT(CLASSNAME) struct CLASSNAME; \
    inline bool CLASSNAME##Registered = \
    (\
        (ComponentManager::getInstance().autoRegister<CLASSNAME>()), \
        true \
    ); \
    struct CLASSNAME
