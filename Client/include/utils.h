#pragma once

#include <iostream>
#include <stack>

bool answer_is_correct( std::string expr, std::string answer, std::string & )
{
    std::stack<double> values;
    char prev_op = '+';
    std::string num_str;

    for ( size_t i = 0; i <= expr.size(); ++i ) {
        char c = ( i < expr.size() ) ? expr[i] : '\0';

        if ( isdigit( c ) ) {
            num_str += c;
        }

        if ( ! isdigit( c ) || i == expr.size() ) {
            if ( ! num_str.empty() ) {
                double num = std::stod( num_str );
                num_str.clear();

                if ( prev_op == '+' ) {
                    values.push( num );
                } else if ( prev_op == '-' ) {
                    values.push( -num );
                } else if ( prev_op == '*' ) {
                    double top = values.top();
                    values.pop();
                    values.push( top * num );
                } else if ( prev_op == '/' ) {
                    double top = values.top();
                    values.pop();
                    values.push( top / num );
                }
            }

            prev_op = c;
        }
    }

    double result = 0;
    while ( ! values.empty() ) {
        result += values.top();
        values.pop();
    }

    if ( std::to_string( result ) == answer ) {
        return true;
    } else {
        return false;
    }
}