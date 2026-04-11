#include "cli.hpp"
#include "utils.hpp"
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

auto main() -> int {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    CLI cli;

    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream stream(line);
        std::string command;
        stream >> command;

        if (command == "uci") cli.uci(stream);
        else if (command == "isready") cli.readyok(stream);
        else if (command == "ucinewgame") cli.ucinewgame(stream);
        else if (command == "position") cli.position(stream);
        else if (command == "go") cli.go(stream);
        else if (command == "stop") cli.stop(stream);
        else if (command == "quit") cli.quit(stream);
        else if (command == "d") printBoard(cli.board);
    }
    return 0;
}