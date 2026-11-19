cmake_minimum_required(VERSION 3.16)
project(my_server VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # clangd / VS Code IntelliSense
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# the "node_modules" of this project (install them once: see README)
find_package(expresspp CONFIG REQUIRED)     # Express++ (+ dotenv, morgan, cors, helmet, jwt, bcrypt, multer, crypto, cookie-parser ...)
find_package(nevla CONFIG REQUIRED)         # mongoose
find_package(axiospp CONFIG REQUIRED)       # axios
find_package(nodemailerpp CONFIG REQUIRED)  # nodemailer
find_package(socketiopp CONFIG REQUIRED)    # socket.io
find_package(bullmqpp CONFIG REQUIRED)      # bullmq
find_package(cpp-telegram-bot-api CONFIG REQUIRED)  # node-telegram-bot-api

# everything except main.cc goes into a library, so tests can start the same app
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS src/*.cc)
list(FILTER APP_SOURCES EXCLUDE REGEX ".*/src/main\\.cc$")
add_library(server_core STATIC ${APP_SOURCES})
target_include_directories(server_core PUBLIC src)
target_compile_options(server_core PRIVATE -Wall -Wextra -Wno-missing-field-initializers)  # designated initializers leave fields at their defaults on purpose
target_link_libraries(server_core PUBLIC
  expresspp::expresspp expresspp::nevla
  axiospp::axiospp nodemailerpp::nodemailerpp socketiopp::socketiopp bullmqpp::bullmqpp
  cpp-telegram-bot-api::cpp-telegram-bot-api)

add_executable(server src/main.cc)
target_link_libraries(server PRIVATE server_core)

option(BUILD_TESTS "Build the API tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_executable(api_tests tests/api.test.cc)
  target_link_libraries(api_tests PRIVATE server_core)
  add_test(NAME api_tests COMMAND api_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
