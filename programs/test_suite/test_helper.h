#ifndef OS_TEST_HELPER_H
#define OS_TEST_HELPER_H
#include "utility/print.h"

#define ASSERT_INTERNAL( condition, fmt, ... ) do{ if( !(condition) ) { print( "Assertion %s failed at %s:%u\n\n" #fmt "\n\n", #condition, __FILE__, __LINE__, ##__VA_ARGS__ ); return false; } } while(0)

#define ASSERT_TRUE( a ) ASSERT_INTERNAL( (a), "Expected %s to evaluate true", #a )
#define ASSERT_FALSE( a ) ASSERT_INTERNAL( !(a), "Expected %s to evaluate false", #a )
// TODO Can we make these not side-affecty?
#define ASSERT_EQ( a, b ) ASSERT_INTERNAL( (a) == (b), "Expected %s (=%u) to equal %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define ASSERT_NE( a, b ) ASSERT_INTERNAL( (a) != (b), "Expected %s (=%u) to not equal %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define ASSERT_GE( a, b ) ASSERT_INTERNAL( (a) >= (b), "Expected %s (=%u) to be greater than %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define ASSERT_LE( a, b ) ASSERT_INTERNAL( (a) <= (b), "Expected %s (=%u) to be less than %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )

extern bool error_flag;
#define EXPECT_INTERNAL( condition, fmt, ... ) do{ if( !(condition) ) { print( "Assertion %s failed at %s:%u\n\n" #fmt "\n\n", #condition, __FILE__, __LINE__, ##__VA_ARGS__ ); error_flag = true; } } while(0)

#define EXPECT_TRUE( a ) EXPECT_INTERNAL( (a), "Expected %s to evaluate true", #a )
#define EXPECT_FALSE( a ) EXPECT_INTERNAL( !(a), "Expected %s to evaluate false", #a )
// TODO Can we make these not side-affecty?
#define EXPECT_EQ( a, b ) EXPECT_INTERNAL( (a) == (b), "Expected %s (=%u) to equal %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define EXPECT_NE( a, b ) EXPECT_INTERNAL( (a) != (b), "Expected %s (=%u) to not equal %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define EXPECT_GE( a, b ) EXPECT_INTERNAL( (a) >= (b), "Expected %s (=%u) to be greater than %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )
#define EXPECT_LE( a, b ) EXPECT_INTERNAL( (a) <= (b), "Expected %s (=%u) to be less than %s (=%u)", #a, (uint32_t)a, #b, (uint32_t)b )

#endif //OS_TEST_HELPER_H
