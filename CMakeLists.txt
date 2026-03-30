cmake_minimum_required(VERSION 3.28)
project(openCVTest)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опционально: указываем режим сборки
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Указываем путь к установленной OpenCV (после make install)
# Стандартный путь при установке: /usr/local
# Если вы устанавливали в другую директорию, укажите её здесь
# set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")

# Ищем OpenCV
find_package(OpenCV REQUIRED)

# Проверка, что OpenCV найден
if(OpenCV_FOUND)
    message(STATUS "OpenCV found!")
    message(STATUS "  Version: ${OpenCV_VERSION}")
    message(STATUS "  Include dirs: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${OpenCV_LIBS}")
else()
    message(FATAL_ERROR "OpenCV not found. Please check installation path.")
endif()

# Добавляем исполняемый файл
add_executable(openCVTest 
    main.cpp 
    detection/detection.cpp
    )

# Подключаем заголовочные файлы OpenCV
target_include_directories(openCVTest PUBLIC 
    ${OpenCV_INCLUDE_DIRS}
    detection    
)

# Подключаем библиотеки OpenCV
target_link_libraries(openCVTest ${OpenCV_LIBS})