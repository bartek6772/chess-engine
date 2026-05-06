#pragma once
#include "board.hpp"
#include "searcher.hpp"
#include <atomic>
#include <memory>
#include <thread>

struct CLI {
    Board board;
    std::unique_ptr<Searcher> current_search = nullptr;
    std::atomic<bool> is_searching = false;
    // std::thread search_thread;

    void uci(std::stringstream& stream);
    void readyok(std::stringstream& stream);
    void ucinewgame(std::stringstream& stream);
    void position(std::stringstream& stream);
    void go(std::stringstream& stream);
    void stop(std::stringstream& stream);
    void quit(std::stringstream& stream);
};
