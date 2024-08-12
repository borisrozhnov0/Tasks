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



