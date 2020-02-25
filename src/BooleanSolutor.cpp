#include "BooleanSolutor.h"

#include <exception>
#include <cstdint>
#include <cstring>

element_t or_                    = 0;
element_t and_                   = 1;
element_t not_                   = 2;
element_t open_par_              = 3;
element_t close_par_             = 4;
element_t true_                  = 5;
element_t false_                 = 6;
element_t variable_              = 7;
element_t eq_                    = 8;
element_t ne_                    = 9;
element_t gt_                    = 10;
element_t lt_                    = 11;
element_t ge_                    = 12;
element_t le_                    = 13;
element_t start_variables_space_ = 128;

BooleanSolutor::BooleanSolutor() {
    operators[eq_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[ne_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[gt_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[lt_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[ge_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[le_] = std::make_pair<priority_t, associativity_t>(0, associativity_t::left);
    operators[or_] = std::make_pair<priority_t, associativity_t>(1, associativity_t::left);
    operators[and_] = std::make_pair<priority_t, associativity_t>(2, associativity_t::left);
    operators[not_] = std::make_pair<priority_t, associativity_t>(3, associativity_t::right);

    next_free_variable = start_variables_space_;
}

void BooleanSolutor::on_operator(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops) {
    while(!ops.empty()) {
        auto top = ops.top();
        if(top == open_par_) {
            break;
        }
        if(operators[top].first > operators[x].first || (operators[top].first > operators[x].first && operators[x].second == associativity_t::left)) {
            ops.pop();
            out.push_back(top);
        } else {
            break;
        }
    }
    ops.push(x);
}

void BooleanSolutor::on_open_parentheses(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops) {
    ops.push(x);
}

void BooleanSolutor::on_close_parentheses(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops) {
    while(!ops.empty()) {
        auto top = ops.top();
        ops.pop();
        if(top == open_par_) {
            break;
        } else {
            out.push_back(top);
        }
    }
}

void BooleanSolutor::on_symbol(element_t x, std::vector<element_t> &out, std::stack<element_t> &ops) {
    out.push_back(x);
}

bool BooleanSolutor::solve(std::string const &in) {
    auto normalized_expression = initialize_expression(in);
    auto postfix_expression = infix_to_postfix(normalized_expression);
    return solve_postfix(postfix_expression);
}

#include <iostream>
std::vector<element_t> BooleanSolutor::initialize_expression(std::string const &in) {
    std::vector<element_t> res;

    auto i = in.begin();
    auto e = in.end();

    auto match_one = [&i, &e, &res](char const c, element_t const element) -> bool {
        if(i != e && *i == c) {
            res.push_back(element);
            ++i;
            return true;
        }
        return false;
    };

    auto match_two = [&i, &e, &res](char const c1, char const c2, element_t const element) -> bool {
        if(i != e && i+1 != e && *i == c1 && *(i+1) == c2) {
            res.push_back(element);
            i+=2;
            return true;
        }
        return false;
    };

    auto match_number = [&i, &e, &res, this]() -> bool {
        try {
            size_t pos = 0;
            auto x = stod(std::string(i, e), &pos);
            if(pos == 0) {
                return false;
            }

            res.push_back(static_cast<element_t>(next_free_variable));
            variables[next_free_variable] = x;
            next_free_variable++;

            i += pos;
            return true;
        } catch(...) {
            return false;
        }
    };

    auto match_boolean = [&i, &e, &res, this]() -> bool {
        auto tlen = strlen("true");
        auto flen = strlen("false");

        if(e - i >= tlen) {
            std::string t(i, i + tlen);
            if(t == "true") {
                res.push_back(true_);
                i += tlen;
                return true;
            }
        }

        if(e - i >= flen) {
            std::string f(i, i + flen);
            if(f == "false") {
                res.push_back(false_);
                i += flen;
                return true;
            }
        }

        return false;
    };

    while(i != e) {
        // ignore spaces
        if(*i == ' ' || *i == '\t') {
            i++;
            continue;
        }

        if(match_number()) {
            continue;
        }

        if(match_boolean()) {
            continue;
        }

        // check double character

        if(match_two('|', '|', or_)) {
            continue;
        }

        if(match_two('&', '&', and_)) {
            continue;
        }

        if(match_two('=', '=', eq_)) {
            continue;
        }

        if(match_two('!', '=', ne_)) {
            continue;
        }

        if(match_two('>', '=', ge_)) {
            continue;
        }

        if(match_two('<', '=', le_)) {
            continue;
        }

        // check single character

        if(match_one('>', gt_)) {
            continue;
        }

        if(match_one('<', lt_)) {
            continue;
        }

        if(match_one('!', not_)) {
            continue;
        }

        if(match_one('(', open_par_)) {
            continue;
        }

        if(match_one(')', close_par_)) {
            continue;
        }

        // nothing matches
        throw std::invalid_argument("invalid symbol in given expression");
    }

    return res;
}

std::vector<element_t> BooleanSolutor::infix_to_postfix(std::vector<element_t> const &in) {
    std::vector<element_t> out;
    std::stack<element_t> ops;

    for(auto x : in) {
        if(x == and_ || x == or_ || x == not_ || x == eq_ || x == ne_ || x == gt_ || x == lt_ || x == ge_ || x == le_) {
            on_operator(x, out, ops);
        } else if(x == open_par_) {
            on_open_parentheses(x, out, ops);
        } else if(x == close_par_) {
            on_close_parentheses(x, out, ops);
        } else if (x == true_ || x == false_) {
            on_symbol(x, out, ops);
        } else if(x >= start_variables_space_) {
            on_symbol(x, out, ops);
        } else {
            throw std::invalid_argument("invalid symbol in infix expression");
        }
    }

    while(!ops.empty()) {
        auto top = ops.top();
        ops.pop();
        out.push_back(top);
    }

    return out;
}

bool BooleanSolutor::solve_postfix(std::vector<element_t> const &in) {
    std::stack<element_t> s;

    auto pop_boolean = [](std::stack<element_t> &s) -> bool {
        if(s.empty()) {
            throw std::invalid_argument("invalid logical expression");
        }

        auto x = s.top();
        s.pop();

        if(x == true_) return true;
        if(x == false_) return false;

        throw std::invalid_argument("invalid logical expression");
    };

    auto push_boolean = [](bool x, std::stack<element_t> &s) {
        if(x) {
            s.push(true_);
        }
        else {
            s.push(false_);
        }
    };

    auto pop_double = [this](std::stack<element_t> &s) -> double {
        if(s.empty()) {
            throw std::invalid_argument("invalid logical expression");
        }

        auto x = s.top();
        s.pop();

        if(variables.find(x) == variables.end()) {
            throw std::invalid_argument("variable not found");
        } else {
            return variables[x];
        }
    };

    auto push_double = [](element_t x, std::stack<element_t> &s) {
        s.push(x);
    };

    for(auto x : in) {
        if(x == and_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto a = pop_boolean(s);
            auto b = pop_boolean(s);
            push_boolean(a && b, s);
            continue;
        }

        if(x == or_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto a = pop_boolean(s);
            auto b = pop_boolean(s);
            push_boolean(a || b, s);
            continue;
        }

        if(x == not_) {
            if(s.size() < 1) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto a = pop_boolean(s);
            push_boolean(!a, s);
            continue;
        }

        if(x == eq_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a == b, s);
            continue;
        }

        if(x == ne_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a != b, s);
            continue;
        }

        if(x == gt_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a > b, s);
            continue;
        }

        if(x == lt_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a < b, s);
            continue;
        }

        if(x == ge_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a >= b, s);
            continue;
        }

        if(x == le_) {
            if(s.size() < 2) {
                throw std::invalid_argument("invalid logical expression");
            }
            auto b = pop_double(s);
            auto a = pop_double(s);
            push_boolean(a <= b, s);
            continue;
        }

        if(x == true_) {
            push_boolean(true, s);
            continue;
        }

        if(x == false_) {
            push_boolean(false, s);
            continue;
        }

        if(x >= start_variables_space_) {
            push_double(x, s);
            continue;
        }

        throw std::invalid_argument("invalid symbol in postifx expression");
    }

    auto res = s.top();
    if(res == true_) {
        return true;
    }
    if(res == false_) {
        return false;
    }

    throw std::invalid_argument("invalid logical expression");
}

