cmake_minimum_required(VERSION 3.31)
project(NodeEdit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Jeśli raylib jest zainstalowane przez system (np. brew lub apt)
find_package(raylib REQUIRED)


# Dodajemy main.cpp jako główny plik źródłowy
add_executable(NodeEdit main.cpp
        Node.cpp
        Node.h)

# Linkowanie raylib i jego zależności
target_link_libraries(NodeEdit raylib ${SNDFILE_LIBRARIES})

# Opcjonalnie (przy niektórych instalacjach) trzeba dodać dodatkowe biblioteki:
# macOS (OpenGL i inne):
if(APPLE)
    target_link_libraries(NodeEdit "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    target_compile_options(NodeEdit PRIVATE -fsanitize=undefined)
    target_link_options(NodeEdit PRIVATE -fsanitize=undefined)
endif()

# Linux (czasem trzeba dołączyć dodatkowe biblioteki ręcznie):
# target_link_libraries(NodeEdit m pthread dl GL X11 Xrandr Xinerama Xi Xxf86vm Xcursor)