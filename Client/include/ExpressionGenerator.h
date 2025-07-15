#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>

class ExpressionGenerator {
public:
    ExpressionGenerator() = default;
    ~ExpressionGenerator() = default;

    void generate( size_t );
    std::string get_expr() const;
    std::vector<std::string> get_parts_expr() const;


private:
    std::string m_expr;

    std::string get_rand_operand() const;
    const char get_rand_operator() const;
};