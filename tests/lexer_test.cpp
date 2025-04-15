#include "lexer.hpp" // Include your project headers
#include <gtest/gtest.h>

//========================================================================
// Test Fixtures
//========================================================================
struct TestLexerSources : public ::testing::Test {

    // Can add multiple cases to test

  protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestLexerSources, sample_test) { ASSERT_EQ(test(), 1); }