#include "chess.h"
#include <gtest/gtest.h>
#include <cstdio>
#include <iostream>

TEST(ChessTest, All) {
    printf("hello wrold\n");

    chess::GameState state;
    chess::Response res;

    res= state.execute("move 0 1 0 3");
    EXPECT_EQ(res, chess::Response::Success);

    res= state.execute("move 0 3 0 4");
    EXPECT_EQ(res, chess::Response::ErrNotYourTurn);

    res= state.execute("move 1 6 1 5");
    EXPECT_EQ(res, chess::Response::Success);
 
    res= state.execute("move 0 3 0 5");
    EXPECT_EQ(res, chess::Response::ErrPawnMove);
   
    res= state.execute("move 0 0 0 5");
    std::cout << chess::responseTostr(res) << std::endl;
    EXPECT_EQ(res, chess::Response::ErrBlocked);

}
