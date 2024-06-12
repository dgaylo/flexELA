#include "../element.h"
#include <gtest/gtest.h>

constexpr svec::Element TEST_A = {23, 0.1};

// constexpr svec::Label TEST_LABEL_A=23;
// constexpr svec::Value TEST_VALUE_A=0.1;

TEST(ElementTests, Assignment)
{
    svec::Element e = TEST_A;

    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_EQ(e.v, TEST_A.v);
}

TEST(ElementTests, Assignment_Value)
{
    svec::Element e;

    // addition
    e = TEST_A;
    e += 0.2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v + 0.2);

    // subtraction
    e = TEST_A;
    e -= 0.2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v - 0.2);

    // multiplication
    e = TEST_A;
    e *= 0.2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v * 0.2);

    // division
    e = TEST_A;
    e /= 0.2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v / 0.2);
}

TEST(ElementTests, Assignment_Element)
{
    svec::Element e;
    svec::Element e2 = {TEST_A.l, 0.3};

    // addition
    e = TEST_A;
    e += e2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v + 0.3);

    // subtraction
    e = TEST_A;
    e -= e2;
    EXPECT_EQ(e.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e.v, TEST_A.v - 0.3);
}

TEST(ElementTests, isEnd)
{
    svec::Element e = TEST_A;

    EXPECT_FALSE(e.isEnd());

    e = svec::END_ELEMENT;

    EXPECT_TRUE(e.isEnd());
}

TEST(ElementTests, Addition)
{
    svec::Element e = TEST_A;
    svec::Element e2 = {TEST_A.l, 0.3};

    svec::Element e3 = e + e2;

    EXPECT_EQ(e3.l, TEST_A.l);
    EXPECT_EQ(e3.v, TEST_A.v + 0.3);
}

TEST(ElementTests, Subtraction)
{
    svec::Element e = TEST_A;
    svec::Element e2 = {TEST_A.l, 0.3};

    svec::Element e3 = e - e2;

    EXPECT_EQ(e3.l, TEST_A.l);
    EXPECT_EQ(e3.v, TEST_A.v - 0.3);
}

TEST(ElementTests, Multiplication)
{
    svec::Element e = TEST_A;

    svec::Element e3 = e * 0.5;

    EXPECT_EQ(e3.l, TEST_A.l);
    EXPECT_EQ(e3.v, TEST_A.v * 0.5);
}

TEST(ElementTests, FMA)
{
    svec::Element e1 = TEST_A;
    svec::Element e2 = {TEST_A.l, 0.3};
    svec::Value C = 0.98;

    svec::Element e3 = svec::fma(e1, C, e2);

    EXPECT_EQ(e3.l, TEST_A.l);
    EXPECT_DOUBLE_EQ(e3.v, e1.v * C + e2.v);
}

TEST(ElementDeathTest, Addition)
{
    svec::Element e = TEST_A;
    svec::Element e2 = {TEST_A.l + 1, TEST_A.v};

    EXPECT_DEBUG_DEATH(e += e2, "");
    EXPECT_DEBUG_DEATH(e + e2, "");
}

TEST(ElementDeathTest, Subtraction)
{
    svec::Element e = TEST_A;
    svec::Element e2 = {TEST_A.l + 1, TEST_A.v};

    EXPECT_DEBUG_DEATH(e -= e2, "");
    EXPECT_DEBUG_DEATH(e - e2, "");
}
