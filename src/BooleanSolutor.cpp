#include "BooleanSolutor.h"

#include <exception>

BooleanSolutor::BooleanSolutor() {
    operators['+'] = std::make_pair<int, Associativity>(0, Associativity::Left);
    operators['*'] = std::make_pair<int, Associativity>(1, Associativity::Left);
    operators['!'] = std::make_pair<int, Associativity>(2, Associativity::Right);
}

void BooleanSolutor::on_ws(char x, std::stringstream &out, std::stack<char> &ops) {
    // ignore
}

void BooleanSolutor::on_operator(char x, std::stringstream &out, std::stack<char> &ops) {
    while(!ops.empty()) {
        auto top = ops.top();
        if(top == '(') {
            break;
        }
        if(operators[top].first > operators[x].first || (operators[top].first > operators[x].first && operators[x].second == Associativity::Left)) {
            ops.pop();
            out << top;
        } else {
            break;
        }
    }
    ops.push(x);
}

void BooleanSolutor::on_open_parentheses(char x, std::stringstream &out, std::stack<char> &ops) {
    ops.push(x);
}

void BooleanSolutor::on_close_parentheses(char x, std::stringstream &out, std::stack<char> &ops) {
    while(!ops.empty()) {
        auto top = ops.top();
        ops.pop();
        if(top == '(') {
            break;
        } else {
            out << top;
        }
    }
}

void BooleanSolutor::on_symbol(char x, std::stringstream &out, std::stack<char> &ops) {
    out << x;
}

bool BooleanSolutor::solve(std::string const &in) {
    auto postfix = infix_to_postfix(in);
    return solve_postfix(postfix);
}

std::string BooleanSolutor::infix_to_postfix(std::string const &in) {
    std::stringstream out;
    std::stack<char> ops;

    for(auto x : in) {
        switch(x) {
            case ' ':
            case '\t':
                on_ws(x, out, ops);
                break;
            case '*':
            case '+':
            case '!':
                on_operator(x, out, ops);
                break;
            case '(':
                on_open_parentheses(x, out, ops);
                break;
            case ')':
                on_close_parentheses(x, out, ops);
                break;
            case '1':
            case '0':
                on_symbol(x, out, ops);
                break;
            default:
                throw std::invalid_argument("invalid symbol");
                break;
        }
    }

    while(!ops.empty()) {
        auto top = ops.top();
        ops.pop();
        out << top;
    }

    return out.str();
}

bool BooleanSolutor::solve_postfix(std::string const &in) {
    std::stack<char> s;

    for(auto x : in) {
        switch(x) {
            case '*': {
                if(s.size() < 2) {
                    throw std::invalid_argument("invalid logical expression");
                }
                auto a = s.top(); s.pop();
                auto b = s.top(); s.pop();
                s.push(a && b);
                break;
            }
            case '+': {
                if(s.size() < 2) {
                    throw std::invalid_argument("invalid logical expression");
                }
                auto a = s.top(); s.pop();
                auto b = s.top(); s.pop();
                s.push(a || b);
                break;
            }
            case '!': {
                if(s.size() < 1) {
                    throw std::invalid_argument("invalid logical expression");
                }
                auto a = s.top(); s.pop();
                s.push(!a);
                break;
            }
            case '1': {
                s.push(true);
                break;
            }
            case '0': {
                s.push(false);
                break;
            };
            default: {
                if(s.size() < 2) {
                    throw std::invalid_argument("invalid symbol");
                }
            }
        }
    }

    return s.top();
}
