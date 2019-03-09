#!/bin/sh
#-fsanitize=address
g++  uaf.cpp -w -lssh2 -std=c++11 -g -fsanitize=address -g  
