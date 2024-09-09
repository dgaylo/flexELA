#include "../svector.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <limits>

constexpr svec::Label TEST_LABEL_A = 23;
constexpr svec::Value TEST_VALUE_A = 0.1;

svec::Value fRand(svec::Value fMin, svec::Value fMax)
{
    svec::Value f = (svec::Value)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

TEST(SVectorTests, EmptyConstructor)
{
    svec::SVector s = svec::SVector();

    EXPECT_EQ(s.NNZ(), 0);
}

TEST(SVectorTests, SingleElementConstructor)
{
    svec::Element e = {TEST_LABEL_A, TEST_VALUE_A};
    svec::SVector s = svec::SVector(e);

    EXPECT_EQ(s.NNZ(), 1);

    const svec::Element* data = s.data();
    EXPECT_EQ(data[0].l, TEST_LABEL_A);
    EXPECT_EQ(data[0].v, TEST_VALUE_A);
}

TEST(SVectorTests, CopyConstructor)
{
    svec::Element e = {TEST_LABEL_A, TEST_VALUE_A};
    const svec::SVector s = svec::SVector(e);

    svec::SVector s2 = svec::SVector(s);

    EXPECT_EQ(s2.NNZ(), 1);

    const svec::Element* data = s2.data();
    EXPECT_EQ(data[0].l, TEST_LABEL_A);
    EXPECT_EQ(data[0].v, TEST_VALUE_A);
}

TEST(SVectorTests, MoveConstructor)
{
    svec::Element e = {TEST_LABEL_A, TEST_VALUE_A};
    const svec::SVector s = svec::SVector(e);

    svec::SVector s2 = svec::SVector(std::move(s));

    EXPECT_EQ(s2.NNZ(), 1);

    const svec::Element* data = s2.data();
    EXPECT_EQ(data[0].l, TEST_LABEL_A);
    EXPECT_EQ(data[0].v, TEST_VALUE_A);
}

TEST(SVectorTests, MultiElementConstructor)
{
    constexpr size_t length = 5;

    // create a buffer with two svectors in it
    svec::Element* buff = new svec::Element[length + 1];
    for (std::size_t i = 0; i < length; i++) {
        buff[i].l = i;
        buff[i].v = fRand(0.0, 1.0);
    }
    buff[length] = svec::END_ELEMENT;

    svec::SVector s = svec::SVector(buff);

    EXPECT_EQ(s.NNZ(), length);

    for (std::size_t i = 0; i < length; i++) {
        EXPECT_EQ(buff[i].l, s.data()[i].l);
        EXPECT_EQ(buff[i].v, s.data()[i].v);
    }

    delete[] buff;
}

TEST(SVectorTests, Iterator)
{
    constexpr size_t length = 100;

    svec::Element* buff = new svec::Element[length + 1];
    svec::Value sum = 0.0;
    svec::Value sum2 = 0.0;

    for (std::size_t i = 0; i < length; i++) {
        buff[i].l = i;
        buff[i].v = fRand(0.0, 1.0);
        sum += buff[i].v;
        sum2 += buff[i].v * buff[i].v;
    }
    buff[length] = svec::END_ELEMENT;

    svec::SVector s = svec::SVector(buff);
    delete[] buff;

    svec::Value sum_test = 0.0;
    svec::Value sum2_test = 0.0;

    for (auto& elm : s) {
        sum_test += elm.v;
        sum2_test += elm.v * elm.v;
    }

    EXPECT_DOUBLE_EQ(sum, sum_test);
    EXPECT_DOUBLE_EQ(sum2, sum2_test);
}

TEST(SVectorTests, Sum)
{
    constexpr size_t length = 5;

    svec::Element* buff = new svec::Element[length + 1];
    svec::Value sum = 0.0;

    for (std::size_t i = 0; i < length; i++) {
        buff[i].l = i;
        buff[i].v = fRand(0.0, 1.0);
        sum += buff[i].v;
    }
    buff[length] = svec::END_ELEMENT;

    EXPECT_DOUBLE_EQ(svec::SVector(buff).sum(), sum);
    delete[] buff;

    // confirm nothing weird happens with empty svectors
    EXPECT_EQ(svec::SVector().sum(), 0.0);
    EXPECT_EQ(svec::SVector(svec::Element({1, 0.0})).sum(), 0.0);
}

TEST(SVectorTests, Add)
{
    svec::Element buff1[4] = {{1, 0.1}, {3, 0.2}, {4, 0.8}, svec::END_ELEMENT};
    svec::SVector s1 = svec::SVector(buff1);

    svec::Element buff2[4] = {{2, 0.8}, {3, 0.3}, {8, 0.4}, svec::END_ELEMENT};
    svec::SVector s2 = svec::SVector(buff2);

    svec::Element solution[5] = {
        {1, 0.1 * 0.5}, {2, 0.0 * 0.5 + 0.8}, {3, 0.2 * 0.5 + 0.3}, {4, 0.8 * 0.5 + 0.0}, {8, 0.4}};

    s2.add(s1, 0.5);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(s2.data()[i].l, solution[i].l);
        EXPECT_DOUBLE_EQ(s2.data()[i].v, solution[i].v);
    }

    // Try again, but reverse which being multiplied
    svec::Element solution2[5] = {
        {1, 0.1}, {2, 0.8 * 0.6}, {3, 0.3 * 0.6 + 0.2}, {4, 0.8}, {8, 0.4 * 0.6}};
    s2 = svec::SVector(buff2);

    s1.add(s2, 0.6);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(s1.data()[i].l, solution2[i].l);
        EXPECT_DOUBLE_EQ(s1.data()[i].v, solution2[i].v);
    }

    auto ns = svec::NormalizedSVector(s1, 0.5);
    auto sum = s1.sum();
    s1.add(ns, 1.0);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(s1.data()[i].l, solution2[i].l);
        EXPECT_DOUBLE_EQ(s1.data()[i].v, solution2[i].v * (1.0 + 0.5 / sum));
    }
}

TEST(SVectorTests, Chop)
{
    svec::Element buff[7] = {{0, 5},  {1, -0.1}, {3, 0.2},         {4, 0.8},
                             {8, -5}, {9, 0.0},  svec::END_ELEMENT};
    svec::SVector s = svec::SVector(buff);

    s.chop();

    EXPECT_EQ(s.NNZ(), 3);
    EXPECT_DOUBLE_EQ(s.sum(), 5 + 0.2 + 0.8);
}

TEST(SVectorTests, Normalize)
{
    constexpr size_t length = 5;

    svec::Element* buff = new svec::Element[length + 1];
    svec::Value sum = 0.0;

    for (std::size_t i = 0; i < length; i++) {
        buff[i].l = i;
        buff[i].v = fRand(0.0, 1.0);
        sum += buff[i].v;
    }
    buff[length] = svec::END_ELEMENT;

    svec::SVector s = svec::SVector(buff);
    s.normalize(0.3);

    for (std::size_t i = 0; i < length; i++) {
        EXPECT_EQ(buff[i].l, s.data()[i].l);
        EXPECT_DOUBLE_EQ(buff[i].v * (0.3 / sum), s.data()[i].v);
    }

    svec::SVector s2 = svec::NormalizedSVector(svec::SVector(buff), 0.3);
    ASSERT_EQ(s.NNZ(), s2.NNZ());
    for (std::size_t i = 0; i < length; i++) {
        EXPECT_EQ(s.data()[i].l, s2.data()[i].l);
        EXPECT_EQ(s.data()[i].v, s2.data()[i].v);
    }

    // normalizing by zero should give an empty vector
    s = svec::SVector(buff);
    s.normalize(0.0);
    EXPECT_EQ(s.NNZ(), 0);

    s2 = svec::NormalizedSVector(svec::SVector(buff), 0);
    EXPECT_EQ(s2.NNZ(), 0);

    delete[] buff;

    // confirm nothing weird happens with empty vectors
    s = svec::SVector();
    s.normalize();
    EXPECT_EQ(s.NNZ(), 0);
    EXPECT_EQ(s.sum(), 0.0);

    s = svec::SVector(svec::Element({1, 0.0}));
    s.normalize();
    EXPECT_EQ(s.NNZ(), 0);
    EXPECT_EQ(s.sum(), 0.0);

    s2 = svec::NormalizedSVector(svec::Element({1, 0.0}));
    EXPECT_EQ(s2.NNZ(), 0);
    EXPECT_EQ(s2.sum(), 0.0);

    s2 = svec::NormalizedSVector(svec::SVector());
    EXPECT_EQ(s2.NNZ(), 0);
    EXPECT_EQ(s2.sum(), 0.0);

    // make sure using normalize on subnormal values gives zero, not infinity
    svec::Element buff2[3] = {
        {0, std::numeric_limits<svec::Value>::min() / 100.0}, {1, 0}, svec::END_ELEMENT};
    s = svec::SVector(buff2);

    s.normalize(2.0);
    EXPECT_EQ(s.sum(), 0.0);

    EXPECT_EQ(svec::SVector(svec::NormalizedSVector(svec::SVector(buff2), 2.0)).sum(), 0.0);
}

TEST(SVectorTests, FMA)
{
    svec::Element buff1[4] = {{1, 0.1}, {3, 0.2}, {4, 0.8}, svec::END_ELEMENT};
    svec::SVector s1 = svec::SVector(buff1);

    svec::Element buff2[4] = {{2, 0.8}, {3, 0.3}, {8, 0.4}, svec::END_ELEMENT};
    svec::SVector s2 = svec::SVector(buff2);

    svec::Element solution[5] = {
        {1, 0.1 * 0.5}, {2, 0.0 * 0.5 + 0.8}, {3, 0.2 * 0.5 + 0.3}, {4, 0.8 * 0.5 + 0.0}, {8, 0.4}};

    svec::SVector s3 = svec::fma(s1, 0.5, s2);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(s3.data()[i].l, solution[i].l);
        EXPECT_DOUBLE_EQ(s3.data()[i].v, solution[i].v);
    }

    // Try again, but reverse which being multiplied
    svec::Element solution2[5] = {
        {1, 0.1}, {2, 0.8 * 0.6}, {3, 0.3 * 0.6 + 0.2}, {4, 0.8}, {8, 0.4 * 0.6}};

    s3 = svec::fma(s2, 0.6, s1);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(s3.data()[i].l, solution2[i].l);
        EXPECT_DOUBLE_EQ(s3.data()[i].v, solution2[i].v);
    }
}

TEST(SVectorTests, zeroEntry)
{
    svec::Element buff[6] = {{0, 0.1}, {1, 0.1}, {3, 0.2}, {4, 0.8}, {6, 0.3}, svec::END_ELEMENT};
    svec::SVector s = svec::SVector(buff);

    // should not change anything
    s.zeroEntry(5);
    ASSERT_EQ(s.NNZ(), 5);

    // should not change anything
    s.zeroEntry(8);
    ASSERT_EQ(s.NNZ(), 5);

    // remove zero at the start
    s.zeroEntry(0);
    ASSERT_EQ(s.NNZ(), 4);
    for (auto i = 0; i < 4; i++) {
        EXPECT_EQ(s.data()[i].l, buff[i + 1].l);
        EXPECT_EQ(s.data()[i].v, buff[i + 1].v);
    }
}

TEST(SVectorTests, ContainsNaN)
{
    svec::Element buff1[4] = {
        {0, 0.1}, {1, std::numeric_limits<svec::Value>::quiet_NaN()}, {3, 0.2}, svec::END_ELEMENT};
    svec::SVector s1 = svec::SVector(buff1);

    svec::Element buff2[4] = {{0, 0.1}, {1, 0.1}, {3, 0.2}, svec::END_ELEMENT};
    svec::SVector s2 = svec::SVector(buff2);

    EXPECT_TRUE(s1.containsNaN());
    EXPECT_FALSE(s2.containsNaN());
}
