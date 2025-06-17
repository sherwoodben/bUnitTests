This is a single-file header for a simple testing "framework."

While everything required for the framework to work is technically contained in the file, some setup for generating the application which runs the test is required.

The test application must link any required libraries, and must not attempt to provide an additional entry point (since the framework already defines one).

To get the application's entry point to be "active," a preprocessor definition for 'bBUILD_TESTS' must exist.

Similar to other "single-file header" libraries, this file may be included in as many other files as required. However, in _exactly one_ .cpp file, before the include directive, one must define 'bTEST_IMPLEMENTATION' like so:

    #define bTEST_IMPLEMENTATION
    #include "bUnitTests.h"


To define a test, simply use the provided macro; for example this test will be named 'one_is_odd' and asserts that one is in fact odd:

    bTEST_FUNCTION(one_is_odd)
    {
        bTEST_ASSERT( 1 % 2 == 1);
    }

That test will pass! The following test, named 'one_is_even' will fail:

    bTEST_FUNCTION(one_is_even)
    {
        bTEST_ASSERT( 1 % 2 == 0);
    }

And when the test application runs, the results of 'one_is_odd' and 'one_is_even' will be printed to std::cout along with some information about which test is currently running.

The application returns 0 if all tests pass, and -1 if any test fails. This allows the application to be used as some sort of tooling for a build step for more complicated projects!

This entire repository need not be cloned every time a project which utilizes this testing framework is created. The single-file can be downloaded to the desired location-- one of the reasons this was developed as a single-file header framework!