#pragma once

//--File Info-----------------------------------------------------------------------------------------------------------

/// @file bUnitTests.h
/// @version 1.3.0
/// @brief "header only" unit testing framework/application
///
/// The "header only" mentioned above is a little bit of a lie. While this file _does_ contain everything that is needed
/// to use/make a unit testing application, it's not enough to have just the file on its own. This file should be
/// included in a project which wants to generate a unit testing program. Then, in _ONE AND ONLY ONE_ .cpp file be sure
/// to define bTEST_IMPLEMENTATION just before including the file, like so:
///
///     #define bTEST_IMPLEMENTATION
///     #include "bUnitTests.h"
///
/// This will make the implemenatation in that .cpp file, but this still isn't enough to build the test application!
///
/// To build the test application, only this file and any source file (for the main project) which contains a test need
/// be included as source files. The unit testing application will also need to link any libraries which the tests
/// will depend on. This file (or at least the implementation) contains an entry point! Therefore, _do NOT_ add the
/// source file which contains the main project's entry point to the list of source files for the unit testing
/// application. Or, be sure to _not_ compile the main project's entry point in the unit testing application if the file
/// which includes the entry point _is_ included in the list of source files.
///
/// For example, consider a project which keeps all of its tests in a directory "/tests/" at the root of the project:
///
///     /example-project/
///         /include/
///         /src/
///         /tests/
///             /include/
///                 bUnitTests.h
///             /test_1.cpp
///             /test_2.cpp
///             ...
///
/// Each of the test files (test_1.cpp, test_2.cpp, etc.) would include the UnitTests header:
///
///     #include "../include/bUnitTests.h"
///
/// And in _exactly one_ of the test files:
///
///     #define bTEST_IMPLEMENTATION
///     #include "../include/bUnitTests.h"
///
/// The (unit testing) application would have as source files _ALL_ files in the "/tests/" directory. It would link
/// any libraries required by the tests.
///
/// The directory layout is not prescriptive! That is, there are plenty of options for where to place this file/test
/// source code files. The tests can live in the implementations, or in a separate tests directory as demonstrated
/// above.
///
/// This file can also be added to the /include/ search path for the compiler/linker, in which case the include
/// might look something like:
///
///     #include "bUnitTests.h"
///
/// (or)
///
///     #include <bUnitTests.h>
///
/// bBUILD_TESTS must be defined to actually build the tests! Otherwise, the entry point/main function is not
/// defined.
///
/// If any of the tests would produce an output to std::cout, they are instead redirected to a log file (default name
/// "tests.txt") which contains the output for each test. The name of the log file can be controlled by providing a
/// definition for bTESTS_LOG_FILE. Logging to a file can be disabled by defining bTESTS_NO_LOG, in which case all
/// outputs are discarded and a log file is not generated -- test results will still print to the console, however.
///
/// Functionality for grouping tests has been provided-- tests are run for each group in sequence, so grouped tests will
/// have their outputs closer together in the log file. Ungrouped tests belong to a group named "ungrouped". To see an
/// example test "implementation", see the bottom of this file (above the license).

//--Changelog---------------------------------------------------------------------------------------------------------//
/*                                                                                                                    //
// This section was introduced in file version 1.1.0 to track the changes which are made to the file.                 //
//                                                                                                                    //
//  v1.3.0  -   Added documentation to the git repo. No changes to this file directly, but wanted to "unify" the      //
//              number in the documentation and the file itself!                                                      //
//                                                                                                                    //
//  v1.2.2  -   Tweaked the doxygen documentation comment describing the variadic arguments (which correspond to the  //
//              optional group name parameter) for the bTEST_FUNCTION macro-- no longer produces a warning/error if   //
//              this file is included the sources for documentation! Also made slight tweaks to the format of some    //
//              areas of this document to move towards a more "unified" style with respect to some other projects I   //
//              am working on.                                                                                        //
//                                                                                                                    //
//              Renamed the log file variable from testsLog to g_testsLog. Fixed method of calling the test functions //
//              to just use the function pointer instead of dereferencing it first. Added documentation to enum       //
//              values. Added explanation for included headers. Added inline keyword to macro so that tests can be    //
//              defined in header files if one feels so inclined. Renamed bTestFunc_return_t to bTestFnResultType.    //
//              Renamed bTestFunc_t to bTestFnType.                                                                   //
//                                                                                                                    //
//              Moved everything (other than macros/"private" implementations/details) into the ben::tests namespace. //
//                                                                                                                    //
//  v1.2.1  -   Fixed a bug which was introduced in v1.2.0 wherein the success of the program (return 0 for success,  //
//              and -1 for failure) would not trigger even if all tests passed. This was because the number of passed //
//              tests was actually being compared to the number of groups due to a change in the structure in the     //
//              previous update. Updated to compare the number of passed tests to the total number of tests!          //
//                                                                                                                    //
//  v1.2.0  -   Introduced capability to group tests by providing an (optional) second parameter to the test function //
//              macro. Tests which are not provided a group (i.e. defined with the old macro) are added to a group    //
//              named "ungrouped"-- if the user also chooses this as their group name the test will be included with  //
//              the other ungrouped tests.                                                                            //
//                                                                                                                    //
//              The (unordered) map of strings to test functions was changed to an (unordered) map of strings to an   //
//              (unordered) map of strings to test functions. The first string is the group "key" while the second    //
//              string is the "function name" key which is how we allow for the grouping of tests. The means of       //
//              adding and accessing the test functions have been updated to account for the extra string key.        //
//                                                                                                                    //
//  v1.1.0  -   Introduced capability to capture the std::cout output from the functions being tested to a log file.  //
//              This reduces clutter in the "status" printouts for testing of functions which may involve printing to //
//              std::cout. The log file is controllable via defining the bTESTS_LOG_FILE macro. If not defined, it    //
//              defaults to "tests.txt"                                                                               //
//                                                                                                                    //
//              The behavior is on by default, but if no log file is desired it can be disabled by providing a        //
//              preprocessor definition for bTESTS_NO_LOG. If this value is defined, now only the status printouts    //
//              appear in the console/output of this program-- any calls to std::cout in the test functions point to  //
//              a nullptr buffer which effectively silences the output.                                               //
//                                                                                                                    //
//              Also, made the g_successes variable static.                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------*/

//--Includes------------------------------------------------------------------------------------------------------------

#include <exception> // the "core" of our testing framework; failing tests are caught via thrown exceptions
#include <string>    // for strings

//--Macros--------------------------------------------------------------------------------------------------------------

// set the file path separator based on the platform...
#ifdef _WIN32
#    define bFILE_PATH_SEPARATOR '\\' ///< file path separator (Windows)
#else
#    define bFILE_PATH_SEPARATOR '/' ///< file path separator (not Windows)
#endif                               // _WIN32

/// @brief test function convenience macro (optionally grouped with a second argument)
///
/// creates a forward declaration to a function which returns a bTestFunc_return_t using the provided "name", then
/// creates a special struct which inherits from the base UnitTest struct and "forwards" the forward-declared function
/// to the UnitTest base struct ctor. Creates a (global, inline, static, const) instance of the specialized class (all
/// within an anonymous namespace to prevent clashing, etc.), then begins the definition of the actual test function
/// which was forward declared! Expects the user to provide the body of the function (in brackets) after the macro
/// appears
///
/// @param fName the "name" of the test function-- can contain any character that is valid in a function signature (not
/// whitespace) including underscores (but it cannot start with a number). Will not compile if the name is not valid!
///
/// @note the variadic arguments are actually a single argument and it is the "group" the test belongs to-- used for
/// organizational purposes and to group similar tests! Can be left blank for ungrouped tests (which is why it's a
/// variadic argument)
#define bTEST_FUNCTION(fName, ...)                                                                                     \
    ben::tests::bTestFnResultType fName##_TestFunc();                                                                  \
    namespace                                                                                                          \
    {                                                                                                                  \
        inline static const struct fName##_UnitTest : public ben::tests::UnitTest                                      \
        {                                                                                                              \
            fName##_UnitTest() : UnitTest{#fName, &fName##_TestFunc, ##__VA_ARGS__} {};                                \
            virtual ~fName##_UnitTest()                               = default;                                       \
            fName##_UnitTest(const fName##_UnitTest &)                = delete;                                        \
            fName##_UnitTest(fName##_UnitTest &&) noexcept            = delete;                                        \
            fName##_UnitTest &operator=(const fName##_UnitTest &)     = delete;                                        \
            fName##_UnitTest &operator=(fName##_UnitTest &&) noexcept = delete;                                        \
        } g_##fName##Test;                                                                                             \
    }                                                                                                                  \
    inline ben::tests::bTestFnResultType fName##_TestFunc()

/// @brief test assertion macro, throws an exception if the argument is not true
///
/// @param expr the expression to evaluate (must be true for the assertion to pass)
#define bTEST_ASSERT(expr)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            std::string  errorString{std::string{__FILE__} + ":" + std::to_string(__LINE__)};                          \
            const size_t lastSeparator{errorString.find_last_of(bFILE_PATH_SEPARATOR)};                                \
            errorString = errorString.substr((lastSeparator == std::string::npos ? 0 : lastSeparator));                \
            throw std::exception{errorString.c_str()};                                                                 \
        }                                                                                                              \
    } while (false)

//--Unit Test Base Class------------------------------------------------------------------------------------------------

namespace ben
{
    namespace tests
    {

        /// @brief the return type for a test function within the framework
        using bTestFnResultType = void;

        /// @brief the type of function to use as a test function within the framework
        using bTestFnType = bTestFnResultType (*)();

        /// @brief a struct which represents a unit test
        /// @note this is intentionally "empty" (aside from a custom ctor) because the way the unit testing framework
        /// implements tests is by inheriting from this (very basic) struct for each test to be implemented. The ctors
        /// of the specific tests provide the arguments for this ctor, so the macro to create each individual test can
        /// (indirectly) provide a name and a function to this ctor and everything plays nicely
        struct UnitTest
        {
          protected:
            /// @brief accepts a name for the unit test (a c str) and a pointer to the function which implements the
            /// test
            /// @param name the name of the test (not necessarily the name of the function)
            /// @param func a pointer to a bTestFunc_t (a function which takes no arguments and returns nothing) which
            /// houses the test implementation
            /// @param group the name of the group the test belongs to
            UnitTest(const char *name, bTestFnType func, const char *group = "ungrouped");

            // we do NOT want to be able to copy/move/assign unit tests (that's nonsensical)
            UnitTest()                                = delete;
            UnitTest(const UnitTest &)                = delete;
            UnitTest(UnitTest &&) noexcept            = delete;
            UnitTest &operator=(const UnitTest &)     = delete;
            UnitTest &operator=(UnitTest &&) noexcept = delete;

          public:
            /// @brief dtor is virtual since unit tests are to be inherited from
            virtual ~UnitTest() = default;
        };
    } // namespace tests
} // namespace ben

//--Implementation------------------------------------------------------------------------------------------------------

// only add the implementations to one single file where bTEST_IMPLEMENTATION is defined
#ifdef bTEST_IMPLEMENTATION
#    include <iostream>      // for printing to console, etc
#    include <unordered_map> // for storing tests such that they can be accessed by group names and then test name
#    ifndef bTESTS_NO_LOG
#        include <fstream> // for outputting to a file
#        ifndef bTESTS_LOG_FILE
/// @brief the default log file name; if bTESTS_LOG_FILE is defined by the user (before this point) then that value is
/// used instead
#            define bTESTS_LOG_FILE "tests.txt"
#        endif // !bTESTS_LOG_FILE
#    endif     // !bTEST_NO_LOG

namespace
{
    //--Implementation Types--------------------------------------------------------------------------------------------

    /// @brief possible return values for the program-- either all the tests passed or at least one test failed!
    enum struct ReturnValue : int
    {
        pass = 0,  ///< all tests passed!
        fail = -1, ///< at least one test failed
    };

    //--Implementation Variables----------------------------------------------------------------------------------------

    // we only need the log file variable if we're making use of the log file
#    ifndef bTESTS_NO_LOG
    /// @brief the output (logging) file stream
    static std::ofstream g_testsLog{bTESTS_LOG_FILE};
#    endif // !bTESTS_NO_LOG

    /// @brief keep track of the number of successes; incremeneted whenever a test passes
    static size_t g_successes{0};

    //--Implementation Methods------------------------------------------------------------------------------------------

    /// @brief gets the unordered map of strings (group names) to unordered map of strings (function names) to test
    /// function pointers to evaluate
    /// @return the (static) unordered map of (group) strings to unordered maps of (function name) string identifiers to
    /// test function pointers
    /// @remark accessed through a "static getter" to avoid static initialization order problems!
    std::unordered_map<std::string, std::unordered_map<std::string, ben::tests::bTestFnType>> &get_tests()
    {
        // store as a static variable in this function, return a reference to it
        static std::unordered_map<std::string, std::unordered_map<std::string, ben::tests::bTestFnType>> s_tests;
        return s_tests;
    };

    /// @brief convenience function; prints an 80 char long dashed line to the output
    void print_line_separator()
    {
        std::cout << "--------------------------------------------------------------------------------\n";
    };

    /// @brief gets the total number of tests as the sum of all test counts per group
    /// @return the total number of tests
    const size_t get_number_of_tests()
    {
        static bool   first{true};
        static size_t numTests{0};

        if (first)
        {
            for (const auto &[group, tests] : get_tests())
            {
                numTests += tests.size();
            }
            first = false;
        }
        return numTests;
    }

    /// @brief prints information regarding the tests which are about to be performed as well as what the return value
    /// of the program indicates
    void print_info()
    {
        print_line_separator();
        std::cout << "INFO:\tIf all tests pass (or no tests fail), the program will return success."
                     "\n\t\tOtherwise, it will return failure.\n";
        std::cout << "INFO:\tFound " << get_number_of_tests() << " test" << (get_number_of_tests() == 1 ? "" : "s")
                  << " in " << get_tests().size() << " group" << (get_tests().size() == 1 ? ".\n" : "s.\n");
        print_line_separator();
    }

    /// @brief actually evaluate the tests
    void run_tests()
    {
        size_t idx{0};

        // save a pointer to the original std::cout buffer...
        std::streambuf *const coutBuffer{std::cout.rdbuf()};

        std::cout << "RUNNING TESTS...\n";

        // walk through each group, getting the tests associated with that group
        for (const auto &[group, tests] : get_tests())
        {
            // announce which group we're running
            std::cout << "Group: '" << group << "'\n";
#    ifndef bTESTS_NO_LOG
            // set the std::cout's rdbuf to the output file stream...
            std::cout.set_rdbuf(g_testsLog.rdbuf());
            std::cout << "Group: '" << group << "'\n";
            // and switch std::cout's rdbuf back to the old value!
            std::cout.set_rdbuf(coutBuffer);
#    endif // !bTESTS_NO_LOG
           // walk through all of the tests and run them:
            for (const auto &[name, test] : tests)
            {
                // announce which test we're running
                std::cout << "\t[" << (idx + 1) << "] : '" << name << "' ";

                // it's feasible the function might try to print to std::cout... but we're printing the result
                // (pass/fail) of the test there. We don't want to pollute the output too much! Instead, we can print
                // the output from each test to a log file (along with a "header" for each test to separate the output
                // of each test). We can switch the target buffer for std::cout to be a file so that anything that is
                // printed to std::cout can be found in the file
                //
                // all that being said, we can skip the logging to a file if the bTESTS_NO_LOG preprocessor macro is
                // defined

#    ifndef bTESTS_NO_LOG
                // set the std::cout's rdbuf to the output file stream...
                std::cout.set_rdbuf(g_testsLog.rdbuf());
                // announce which test we're running (now in the log file)
                print_line_separator();
                std::cout << "Test '" << name << "' log:\n\n";
#    else
                std::cout.set_rdbuf(nullptr);
#    endif // !bTESTS_NO_LOG

                // use exceptions to figure out if tests pass
                try
                {
                    test();
#    ifndef bTESTS_NO_LOG
                    // print a success message and line separator...
                    std::cout << "\npassed.\n";
                    print_line_separator();
#    endif // !bTESTS_NO_LOG
           // and switch std::cout's rdbuf back to the old value!
                    std::cout.set_rdbuf(coutBuffer);
                    std::cout << "passed.\n";
                    g_successes++;
                }

                // catch the exceptions (i.e. a failed test)
                catch (const std::exception &e)
                {
#    ifndef bTESTS_NO_LOG
                    // print the error location and line separator...
                    std::cout << "\nfailed at '" << e.what() << "'.\n";
                    print_line_separator();
#    endif // !bTESTS_NO_LOG
           // and switch std::cout's rdbuf back to the old value!
                    std::cout.set_rdbuf(coutBuffer);
                    std::cout << "failed at '" << e.what() << "'.\n";
                }
                idx++;
            }
        }
    }

    /// @brief prints a summary of the results
    void print_summary()
    {
        print_line_separator();
        std::cout << "SUMMARY:\n";
        std::cout << "\tPassed " << g_successes << " out of " << get_number_of_tests() << " tests.\n";
        print_line_separator();

#    ifndef bTESTS_NO_LOG
        // save a pointer to the original std::cout buffer...
        std::streambuf *const coutBuffer{std::cout.rdbuf()};
        // set the std::cout's rdbuf to the output file stream...
        std::cout.set_rdbuf(g_testsLog.rdbuf());
        print_line_separator();
        std::cout << "SUMMARY:\n";
        std::cout << "\tPassed " << g_successes << " out of " << get_number_of_tests() << " tests.\n";
        print_line_separator();
        // and switch std::cout's rdbuf back to the old value!
        std::cout.set_rdbuf(coutBuffer);
#    endif // !bTESTS_NO_LOG
    }
} // namespace

ben::tests::UnitTest::UnitTest(const char *name, ben::tests::bTestFnType func, const char *group)
{
    // if the group does not exist, we first need to create it...
    std::string groupString{group};
    if (!get_tests().contains(groupString))
    {
        get_tests().insert_or_assign(groupString, std::unordered_map<std::string, ben::tests::bTestFnType>{});
    }

    // insert (or assign) so the value is updated if the same name is passed
    std::string nameString{name};
    get_tests().at(groupString).insert_or_assign(nameString, func);
}

// only compile the main function if we're building the tests
#    ifdef bBUILD_TESTS
/// @brief main function (entry point for unit testing program)
/// @return passing value if all tests pass, failure value if any test fails
int main()
{
    print_info();

    // if no tests are found, we're done
    if (get_tests().size() == 0)
    {
        return static_cast<int>(ReturnValue::pass);
    }

    run_tests();

    print_summary();

    // returns the "pass" value if all tests pass, or the "fail" value if any tests fail
    return (
        g_successes == get_number_of_tests() ? static_cast<int>(ReturnValue::pass)
                                             : static_cast<int>(ReturnValue::fail));
}
#    endif // bBUILD_TESTS
#    undef bTEST_IMPLEMENTATION
#endif // bTEST_IMPLEMENTATION

//--Example Usage-----------------------------------------------------------------------------------------------------//
/*                                                                                                                    //
// DO NOT REMOVE THIS SECTION!                                                                                        //
//                                                                                                                    //
// Let's say we have some function which takes a string and tokenizes it, and then some function which serializes     //
// tokens back into a string. An original input string and a string reconstructed from the tokens generated by that   //
// input string may not be exactly the same, since the tokenizer/stringifier may interpret whitespace or special      //
// characters in different manners depending on input/output. However, when we tokenize this new resulting string,    //
// we should get the same tokens as the first time (i.e. the tokenization/serialization should be repeatable!)        //
//                                                                                                                    //
// It is also possible to "group" tests by supplying an optional second argument to the macro (a string literal).     //
// Tests run for each group in sequence, so the logged output of grouped tests are located closer together in the     //
// log file. Tests which are not provided a group are placed in a group named "ungrouped".                            //
//                                                                                                                    //
// Let our "tokenize" function take a string as an input and return a vector of strings (or tokens).                  //
// Let our "stringify" function take a vector of strings (or tokens) as an input and return a string.                 //
//                                                                                                                    //
//      std::vector<std::string> tokenize(std::string &s) {...};                                                      //
//      std::string              stringify(std::vector<std::string> &tokens) {...};                                   //
//                                                                                                                    //
// (implementations omitted...)                                                                                       //
//                                                                                                                    //
// Let's also say we want to add this to a group named "tokenizing".                                                  //
//                                                                                                                    //
// Then, a possible implementation of a unit test to ensure the expected results would be as follows :                //
//                                                                                                                    //
//      bTEST_FUNCTION(repeatable_tokenization_and_serialization, "tokenizing")                                       //
//      {                                                                                                             //
//          std::string inputString{...};                                                                             //
//                                                                                                                    //
//          auto tokens1 = tokenize(inputString);                                                                     //
//                                                                                                                    //
//          auto toString = stringify(tokens1);                                                                       //
//                                                                                                                    //
//          auto tokens2 = tokenize(toString);                                                                        //
//                                                                                                                    //
//          bTEST_ASSERT(tokens1.size() == tokens2.size());                                                           //
//                                                                                                                    //
//          for (auto idx = 0; idx < tokens1.size(); idx++)                                                           //
//          {                                                                                                         //
//              bTEST_ASSERT(tokens1.at(idx) == tokens2.at(idx));                                                     //
//          }                                                                                                         //
//      }                                                                                                             //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------*/

//--License-----------------------------------------------------------------------------------------------------------//
/*                                                                                                                    //
// DO NOT REMOVE THIS SECTION!                                                                                        //
//                                                                                                                    //
// MIT License                                                                                                        //
//                                                                                                                    //
// Copyright (c) 2025 sherwoodben                                                                                     //
//                                                                                                                    //
// Permission is hereby granted, free of charge, to any person obtaining a copy                                       //
// of this software and associated documentation files (the "Software"), to deal                                      //
// in the Software without restriction, including without limitation the rights                                       //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                                          //
// copies of the Software, and to permit persons to whom the Software is                                              //
// furnished to do so, subject to the following conditions:                                                           //
//                                                                                                                    //
// The above copyright notice and this permission notice shall be included in all                                     //
// copies or substantial portions of the Software.                                                                    //
//                                                                                                                    //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                                         //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                                           //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                                        //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                                             //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                                      //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                                      //
// SOFTWARE.                                                                                                          //
//--------------------------------------------------------------------------------------------------------------------*/
