g++ -m32 -std=c++17 ^
src/app.cpp src/asciiloader.cpp src/audio.cpp src/config.cpp src/fileparser.cpp src/gui.cpp ^
src/menugui.cpp src/main.cpp src/random.cpp src/renderer.cpp src/timer.cpp src/window.cpp src/mapgen.cpp src/miscdraw.cpp ^
src/gamelogic.cpp src/debug.cpp src/localizer.cpp src/client.cpp src/netutil.cpp src/unit.cpp src/structure.cpp src/item.cpp src/effects.cpp ^
-o main.exe -Ic:/SDL32/include -Lc:/SDL32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lws2_32