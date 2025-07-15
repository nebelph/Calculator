#include <gtest/gtest.h>

#include "ExpressionManager.h"

/*
Sent:     108*111*212+170-59
Received: 2541567.000000
Sent:     130*35-30/141*228
Received: 4501.489362
Sent:     243-249+145/232/239
Received: -5.997385
Sent:     34/106-248-34-163
Received: -444.679245
Sent:     131-89+247*149/249
Received: 189.803213
*/

TEST( ExpressionManagerTest, SimpleExpression )
{
    ExpressionManager em( "2+2*2" );
    double answer = em.get_answer();
    EXPECT_DOUBLE_EQ( answer, 6.0 );
}

TEST( ExpressionManagerTest, AllOperations1 )
{
    ExpressionManager em( "108*111*212+170-59" );
    double answer = em.get_answer();
    double tolerance = 1e-3;
    EXPECT_NEAR( answer, 2541567.000, tolerance );
}

TEST( ExpressionManagerTest, AllOperations2 )
{
    ExpressionManager em( "130*35-30/141*228" );
    double answer = em.get_answer();
    double tolerance = 1e-3;
    EXPECT_NEAR( answer, 4501.489, tolerance );
}

TEST( ExpressionManagerTest, AllOperations3 )
{
    ExpressionManager em( "243-249+145/232/239" );
    double answer = em.get_answer();
    double tolerance = 1e-3;
    EXPECT_NEAR( answer, -5.997, tolerance );
}

TEST( ExpressionManagerTest, AllOperations4 )
{
    ExpressionManager em( "34/106-248-34-163" );
    double answer = em.get_answer();
    double tolerance = 1e-3;
    EXPECT_NEAR( answer, -444.679, tolerance );
}

TEST( ExpressionManagerTest, AllOperations5 )
{
    ExpressionManager em( "131-89+247*149/249" );
    double answer = em.get_answer();
    double tolerance = 1e-3;
    EXPECT_NEAR( answer, 189.803, tolerance );
}