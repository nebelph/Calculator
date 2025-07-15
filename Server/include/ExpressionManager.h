#pragma once

#include <iostream>
#include <algorithm>
#include <stack>

class ExpressionManager {
public:
    ExpressionManager( std::string );

    double get_answer() const;
private:
    std::string m_expr;
    double m_answer;

    void prepare_expr();
    void calculate();
};