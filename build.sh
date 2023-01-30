#!/bin/bash

clang++ -std=c++20 main.cpp -I/usr/local/Cellar/sfml/2.5.1_2/include -o Snake -L/usr/local/Cellar/sfml/2.5.1_2/lib -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system;
./Snake
