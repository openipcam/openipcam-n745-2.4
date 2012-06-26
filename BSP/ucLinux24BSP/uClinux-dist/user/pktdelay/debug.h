#ifndef DEBUG_ASSERTS_INCLUDED
#define DEBUG_ASSERTS_INCLUDED

#if (ASSERT_PRECONDITIONS | ASSERT_POSTCONDITIONS | ASSERT_CONDITIONS)
#undef NDEBUG
#endif

#include <stdio.h>
#include <assert.h>

#if (ASSERT_POSTCONDITIONS | ASSERT_CONDITIONS)
#define assert_post(c, m...) 	if(!(c)) { \
	fprintf(stderr, "=======================\n");\
	fprintf(stderr, "| POSTCONDITION FAILED |\n");\
	fprintf(stderr, "=======================\n");\
	fprintf(stderr, ## m); \
	fprintf(stderr, "\n");\
	assert(c); \
	}
#else
#define assert_post(c,m...)
#endif /*ASSERT_POSTCONDITIONS*/

#if (ASSERT_PRECONDITIONS | ASSERT_CONDITIONS)
#define assert_pre(c, m...) 	if (!(c)) { \
	fprintf(stderr, "=======================\n");\
	fprintf(stderr, "| PRECONDITION FAILED |\n");\
	fprintf(stderr, "=======================\n");\
	fprintf(stderr, ## m); \
	fprintf(stderr, "\n");\
	assert(c); \
	}
#else
#define assert_pre(c,m...)
#endif /*ASSERT_PRECONDITIONS*/

#endif /*DEBUG_ASSERTS_INCLUDED*/
