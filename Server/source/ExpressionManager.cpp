#include "ExpressionManager.h"

ExpressionManager::ExpressionManager( std::string expr ) : m_expr( expr )
{
    prepare_expr();
    calculate();
}

double ExpressionManager::get_answer() const
{
    return m_answer;
}

void ExpressionManager::prepare_expr()
{
    size_t index;
    while ( true ) {
        index = m_expr.find( ' ' );
        if ( index != std::string::npos ) {
            m_expr.erase( index, 1 );
        } else {
            break;
        }
    }
}

void ExpressionManager::calculate()
{
    std::stack<double> values;
    char prev_op = '+';
    std::string num_str;

    for ( size_t i = 0; i <= m_expr.size(); ++i ) {
        char c = ( i < m_expr.size() ) ? m_expr[i] : '\0';

        if ( isdigit( c ) ) {
            num_str += c;
        }

        if ( ! isdigit( c ) || i == m_expr.size() ) {
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

    m_answer = result;
}