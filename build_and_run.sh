#!/bin/bash

# Создаем директорию build и переходим в нее
mkdir build
cd build

# Запускаем CMake для генерации Makefile
cmake ..

# Запускаем процесс сборки
cmake --build . --target kp

# Проверяем статус выхода
if [ $? -eq 0 ]; then
    cd ..
    ./bin/kp
else
    cd ..
    echo "Сборка завершилась с ошибкой, программа не будет запущена."
fi
