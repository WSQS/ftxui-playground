#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "folder_menu.h"

using namespace ftxui;
using namespace playground;

int main() {
    const char *command = "ls -l";
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "pipe() failed!" << std::endl;
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "fork() failed!" << std::endl;
        return 1;
    }
    if (pid == 0) {
        // 子进程
        // 关闭管道的读端
        close(pipefd[0]);

        // 将标准输出重定向到管道的写端
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // 执行命令
        execl("/bin/sh", "sh", "-c", command, nullptr);

        // 如果 execl 失败
        std::cerr << "execl() failed!" << std::endl;
        _exit(1);
    } else {
        // 父进程
        // 关闭管道的写端
        close(pipefd[1]);

        // 从管道的读端读取数据
        char buffer[128];
        std::string result;
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0'; // 确保字符串以 null 结尾
            result += buffer;
        } // 关闭管道的读端
        close(pipefd[0]);

        // 等待子进程结束
        wait(nullptr);

        // 输出结果
        std::cout << "Command output:\n" << result << std::endl;
    }
    return 0;
    std::vector<std::shared_ptr<path_data> > path_datas;
    int select = 0;
    std::vector<reference<std::string> > tab_values{};
    auto tab_container = Container::Tab({}, &select);
    add_folder_menu(path_datas, tab_container, select);
    add_folder_menu(path_datas, tab_container, select);
    tab_values = build_tab_value(path_datas);
    auto tab_toggle = playground::Toggle(&tab_values, &select);
    auto container = Container::Vertical({tab_toggle, tab_container});
    std::string log{};
    auto renderer = Renderer(container, [&] {
        return vbox({
                   tab_toggle->Render(),
                   separator(),
                   tab_container->Render() | flex,
                   build_log(log),
               }) | border;
    }) | CatchEvent([&](const Event &event) {
        // add tab
        if (event == Event::Character('a')) {
            add_folder_menu(path_datas, tab_container, select);
            tab_values = build_tab_value(path_datas);
            // move focus to the last tab
            select = static_cast<int>(tab_values.size()) - 1;
            return true;
        }
        // remove current tab
        if (event == Event::Character('d')) {
            remove_folder_menu(path_datas, tab_container, select);
            tab_values = build_tab_value(path_datas);
            // move focus to the last tab
            select = static_cast<int>(tab_values.size()) - 1;
            return true;
        }
        // handel log
        if (event.input().size() >= 3 && event.input().substr(0, 3) == "log") {
            log = event.input().substr(3, event.input().size() - 3);
            return true;
        }
        return false;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    get_screen().Loop(renderer);
    return EXIT_SUCCESS;
}
