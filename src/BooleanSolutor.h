#ifndef BooleanSolutor_H
#define BooleanSolutor_H

#include <string>
#include <stack>
#include <unordered_map>
#include <vector>

using priority_t = uint8_t;
using element_t = uint64_t;

enum class associativity_t {
    left,
    right
};

class BooleanSolutor {
public:
    BooleanSolutor();
    bool solve(std::string const &in);

private:
    std::vector<element_t> initialize_expression(std::string const &in);
    std::vector<element_t> infix_to_postfix(std::vector<element_t> const &in);
    bool solve_postfix(std::vector<element_t> const &in);

    void on_operator(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops);
    void on_open_parentheses(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops);
    void on_close_parentheses(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops);
    void on_symbol(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops);

private:
    std::unordered_map<element_t, std::pair<priority_t, associativity_t>> operators;
    std::unordered_map<element_t, double> variables;
    element_t next_free_variable;
};

#endif // BooleanSolutor_H
