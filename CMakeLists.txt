cmake_minimum_required(VERSION 3.23)
project(deepening1)

set(CMAKE_CXX_STANDARD 14)

# Add header file
set(SDL_HEADERS /usr/local/Cellar/sdl2/2.0.16/include)

include_directories(${SDL_HEADERS})

# Add target link
set(SDL_LINK /usr/local/Cellar/sdl2/2.0.16/lib/libSDL2-2.0.0.dylib)
link_libraries(${SDL_LINK})

add_executable(deepening1 main.cpp Bitboards.h Constants.h)
