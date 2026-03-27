cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS components/esp_littlefs)
# set(
#     EXTRA_COMPONENT_DIRS 
#     ${CMAKE_SOURCE_DIR}/components/drivers
# )
# set(
#     EXTRA_COMPONENT_DIRS 
#     ${CMAKE_SOURCE_DIR}/components/imu
# )

project(esp32_imu)
