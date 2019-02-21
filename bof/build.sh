#!/bin/sh
#-fsanitize=address
g++  bof.cpp -L/usr/local/opt/openssl/lib  -I/usr/local/opt/openssl/include  -lssl -lcrypto  -lsqlite3 -w -std=c++11 -g -fsanitize=address -g 
