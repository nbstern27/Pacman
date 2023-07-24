all: compile link

compile:
	g++ -I src/include -c main.cpp gameData.cpp player.cpp ghost.cpp
link:
	g++ main.o gameData.o player.o ghost.o -o main -L src/lib -l sfml-graphics -l sfml-window -l sfml-system -l sfml-audio -l sfml-network