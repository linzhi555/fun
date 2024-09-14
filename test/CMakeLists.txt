#add_executable(test1  ${CMAKE_CURRENT_LIST_DIR}/test1.cpp)
#add_executable(socket_test  ${CMAKE_CURRENT_LIST_DIR}/socket.cpp)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(test_chess  ${CMAKE_CURRENT_LIST_DIR}/chess_test.cpp ${CMAKE_CURRENT_LIST_DIR}/chess.cpp)
target_link_libraries(test_chess gtest_main gtest pthread)
add_test(NAME ChessTest COMMAND test_chess)
