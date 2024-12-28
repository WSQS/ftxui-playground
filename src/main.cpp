#include <filesystem>
#include <iostream>

#include "folder_menu.h"
#include "boost/asio.hpp"
#include "boost/process/v2.hpp"
using namespace ftxui;
using namespace playground;
namespace bp = boost::process;
namespace asio = boost::asio;
using boost::system::error_code;
int main() {
    // 执行命令并捕获输出
    // 创建 ASIO 执行上下文
    asio::io_context io_context;

    // 使用 popen 启动进程并连接其标准输出
    asio::readable_pipe proc(io_context);
    std::string command = "ls -l";
    FILE* pipe = popen(command.c_str(), "r");
    proc.assign(fileno(pipe));

    // 读取并处理输出
    std::string line;
    error_code ec;

    // 读取剩余行并打印
    while (true) {
        asio::read_until(proc, asio::dynamic_buffer(line), '\n', ec);
        if (ec == asio::error::eof) {
            break; // 读到文件末尾，退出循环
        }
        std::cout << line;
        line.clear(); // 清空缓冲区以读取下一行
    }

    // 关闭管道
    pclose(pipe);
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
