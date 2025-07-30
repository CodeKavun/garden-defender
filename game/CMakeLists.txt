add_executable(
    GardenDefender
    main.cpp
    engine/core.hpp engine/core.cpp
    engine/utils.hpp engine/utils.cpp
    engine/gfx.hpp engine/gfx.cpp
    engine/sequence.hpp engine/sequence.cpp
)
target_link_libraries(GardenDefender PRIVATE raylib nlohmann_json::nlohmann_json)