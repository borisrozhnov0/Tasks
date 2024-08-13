#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "src/string_t.c"
#include <stdio.h>

BOOST_AUTO_TEST_SUITE( string_constr )
BOOST_AUTO_TEST_CASE( string_constr_init)
{
    string_t s = init_void();

    BOOST_TEST(s.size == STRING_INIT_SIZE);
    BOOST_TEST(s.len == 0);

    del(&s);
}

BOOST_AUTO_TEST_CASE( string_constr_del)
{
    string_t s = init_str("Test");
    del(&s);

    BOOST_TEST(s.size == 0);
    BOOST_TEST(s.len == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( string_param )
BOOST_AUTO_TEST_CASE( string_capacity)
{
    string_t s1 = init_str("Test"),
             s2 = init_void(),
             s3 = init_str("Foo, bar, baz, quux.");

    BOOST_TEST(capacity(s1) == 11);
    BOOST_TEST(capacity(s2) == 15);
    BOOST_TEST(capacity(s3) == 11);

    del(&s1);
    del(&s2);
    del(&s3);
}

BOOST_AUTO_TEST_CASE( string_size)
{
    string_t s1 = init_str("Test"),
             s2 = init_void(),
             s3 = init_str("Foo, bar, baz, quux."),
             s4 = init_void();
    del(&s4);

    BOOST_TEST(size(s1) == 16);
    BOOST_TEST(size(s2) == 16);
    BOOST_TEST(size(s3) == 32);
    BOOST_TEST(size(s4) == 0);

    del(&s1);
    del(&s2);
    del(&s3);
}

BOOST_AUTO_TEST_CASE( string_empty)
{
    string_t s1 = init_str("Test"),
             s2 = init_void(),
             s3 = init_str("Foo, bar, baz, quux.");

    BOOST_TEST(empty(s1) == 1);
    BOOST_TEST(empty(s2) == 0);
    BOOST_TEST(empty(s3) == 1);

    del(&s1);
    del(&s2);
    del(&s3);
}

BOOST_AUTO_TEST_CASE( string_length)
{
    string_t s1 = init_str("Test"),
             s2 = init_void(),
             s3 = init_str("Foo, bar, baz, quux.");

    BOOST_TEST(length(s1) == 4);
    BOOST_TEST(length(s2) == 0);
    BOOST_TEST(length(s3) == 20);

    del(&s1);
    del(&s2);
    del(&s3);
}

BOOST_AUTO_TEST_CASE(string_max_size)
{
    string_t s1 = init_str("Test"),
             s2 = init_void(),
             s3 = init_str("Foo, bar, baz, quux.");

    BOOST_TEST(max_size(s1) == 15);
    BOOST_TEST(max_size(s2) == 15);
    BOOST_TEST(max_size(s3) == 31);

    del(&s1);
    del(&s2);
    del(&s3);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( string_pos )

BOOST_AUTO_TEST_CASE(string_at)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test3 = s1.val;
        
    BOOST_CHECK_EQUAL(at(s1, 7), nullptr);
    BOOST_CHECK_EQUAL(at(s1, -1), nullptr);
    BOOST_TEST(at(s1, 0) == correct_s1_test3);

    del(&s1);

    string_t s2 = init_str("Foo, bar, baz, quux.");

    char * correct_s2_test1 = s2.val + 5,
         * correct_s2_test2 = s2.val + 10;
         
    BOOST_TEST(at(s2, 5) == correct_s2_test1);
    BOOST_TEST(at(s2, 10) == correct_s2_test2);

    del(&s2);
}

BOOST_AUTO_TEST_CASE(string_back)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test1 = s1.val + 3;
    char correct_s1_test2 = 't';    

    BOOST_CHECK_EQUAL(back(s1), correct_s1_test1);
    BOOST_TEST(*back(s1) == correct_s1_test2);
    del(&s1);


    string_t s2 = init_str("Foo, bar, baz, quux.");

    char * correct_s2_test1 = s2.val + 19;
    char correct_s2_test2 = '.';  
    BOOST_CHECK_EQUAL(back(s2), correct_s2_test1);
    BOOST_TEST(*back(s2) == correct_s2_test2);
    
    del(&s2);
}

BOOST_AUTO_TEST_CASE(string_begin)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test1 = s1.val;
    char correct_s1_test2 = 'T';    

    BOOST_CHECK_EQUAL(begin(s1), correct_s1_test1);
    BOOST_TEST(*begin(s1) == correct_s1_test2);
    del(&s1);
}

BOOST_AUTO_TEST_CASE(string_end)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test1 = s1.val + s1.len;

    BOOST_CHECK_EQUAL(end(s1), correct_s1_test1);
    del(&s1);
}

BOOST_AUTO_TEST_CASE(string_rbegin)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test1 = back(s1);

    BOOST_TEST(rbegin(s1) == correct_s1_test1);
    del(&s1);
}

BOOST_AUTO_TEST_CASE(string_rend)
{
    string_t s1 = init_str("Test");

    char * correct_s1_test1 = s1.val - 1;

    BOOST_TEST(rend(s1) == correct_s1_test1);
    del(&s1);
}

BOOST_AUTO_TEST_SUITE_END()

