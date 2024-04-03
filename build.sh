#!/bin/bash

# Создаем директорию build и переходим в нее
mkdir build
cd build

# Запускаем CMake для генерации Makefile
cmake ..

# Запускаем процесс сборки
cmake --build . --target kp

cd ..
