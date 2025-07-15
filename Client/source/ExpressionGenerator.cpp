#include "ExpressionGenerator.h"

void ExpressionGenerator::generate( size_t size )
{
    size = size;
    m_expr.clear();
    for ( size_t i = 0; i < size - 1; i++ ) {
        m_expr += get_rand_operand();
        m_expr += get_rand_operator();
    }

    if ( size != 0 )
        m_expr += get_rand_operand();
}

std::string ExpressionGenerator::get_expr() const
{
    return m_expr;
}

std::vector<std::string> ExpressionGenerator::get_parts_expr() const
{
    int len = m_expr.length();

    int number_of_parts = std::rand() % ( len / 2 ) + 1;

    std::vector<std::string> result;
    int base_size = len / number_of_parts;
    int remainder = len % number_of_parts;

    int index = 0;
    for ( int i = 0; i < number_of_parts; ++i ) {
        int current_size = base_size + ( i < remainder ? 1 : 0 );
        result.push_back( m_expr.substr( index, current_size ) );
        index += current_size;
    }

    return result;
}

std::string ExpressionGenerator::get_rand_operand() const
{
    return std::to_string( std::rand() % UINT8_MAX );
}

const char ExpressionGenerator::get_rand_operator() const
{
    std::vector<char> operands { '+', '-', '*', '/' };

    return operands.at( std::rand() % 4 );
}
