
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
    EXPECT_EQ(command_set, stander_set);
}

TEST(filesystem, exists) {
    EXPECT_EQ(filesystem::command::exists("/123"), false);
    EXPECT_EQ(filesystem::stander::exists("/123"), false);
    EXPECT_EQ(filesystem::command::exists("/mnt"), true);
    EXPECT_EQ(filesystem::stander::exists("/mnt"), true);
}

TEST(filesystem, get_parent_directory) {
    std::string file_path = "/mnt";
    filesystem::command::get_parent_directory(file_path);
    EXPECT_EQ(file_path, "/");
    file_path = "/mnt";
    filesystem::stander::get_parent_directory(file_path);
    EXPECT_EQ(file_path, "/");
}

TEST(filesystem, get_file_name) {
    EXPECT_EQ(filesystem::command::get_file_name("/mnt"), std::string("mnt"));
    EXPECT_EQ(filesystem::stander::get_file_name("/mnt"), std::string("mnt"));
}
