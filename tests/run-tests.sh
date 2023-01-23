#!/bin/bash

make all
valgrind ./list
valgrind ./vec
valgrind ./hashmap
make clean
