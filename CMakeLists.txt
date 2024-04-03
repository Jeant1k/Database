cmake_minimum_required(VERSION 3.10)
project(kp)

set(CMAKE_CXX_STANDARD 17)

# Указываем, куда помещать исполняемые файлы
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Находим пакет PkgConfig, который поможет нам найти libpqxx
find_package(PkgConfig)
# Используем PkgConfig для поиска деталей о libpqxx
pkg_check_modules(PQXX REQUIRED libpqxx)

# Добавляем пути к заголовочным файлам libpqxx
include_directories(${PQXX_INCLUDE_DIRS})
# Добавляем пути к библиотекам libpqxx
link_directories(${PQXX_LIBRARY_DIRS})

# Создаем исполняемый файл нашего проекта
add_executable(kp src/main.cpp src/database.cpp src/utils/utils.cpp)

# Связываем наш исполняемый файл с библиотеками libpqxx
target_link_libraries(kp ${PQXX_LIBRARIES})
