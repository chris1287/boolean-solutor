#ifndef BooleanSolutor_H
#define BooleanSolutor_H

#include <string>
#include <stack>
#include <sstream>
#include <unordered_map>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

class BooleanSolutor {
public:
    BooleanSolutor();
    bool solve(std::string const &in);

private:
    std::string infix_to_postfix(std::string const &in);
    bool solve_postfix(std::string const &in);

    void on_ws(char x, std::stringstream &out, std::stack<char> &ops);
    void on_operator(char x, std::stringstream &out, std::stack<char> &ops);
    void on_open_parentheses(char x, std::stringstream &out, std::stack<char> &ops);
    void on_close_parentheses(char x, std::stringstream &out, std::stack<char> &ops);
    void on_symbol(char x, std::stringstream &out, std::stack<char> &ops);

private:
    enum class Associativity {
        Left,
        Right
    };

    std::unordered_map<char, std::pair<int, Associativity>> operators;
};

#endif // BooleanSolutor_H
