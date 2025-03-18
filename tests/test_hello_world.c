#include <criterion/criterion.h>

Test(hello_world, basic_test) {
    cr_assert_eq(1 + 1, 2, "1 + 1 should equal 2");
}