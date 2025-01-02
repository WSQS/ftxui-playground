
#include "filesystem_wrapper.h"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(filesystem, get_directory_content) {
    auto command = filesystem::command::get_directory_content("/");
    auto stander = filesystem::stander::get_directory_content("/");
    // Expect equality.
    EXPECT_EQ(command.size(), stander.size());
    std::set command_set(command.begin(), command.end());
    std::set stander_set(stander.begin(), stander.end());
    for (auto file : command)
        EXPECT_EQ(stander_set.count(file), 1);
    for (auto file : stander)
        EXPECT_EQ(command_set.count(file), 1);
}

TEST(filesystem, exists) {
    EXPECT_EQ(filesystem::command::exists("/123"), false);
    EXPECT_EQ(filesystem::stander::exists("/123"), false);
    EXPECT_EQ(filesystem::command::exists("/mnt"), false);
    EXPECT_EQ(filesystem::stander::exists("/mnt"), false);
}
