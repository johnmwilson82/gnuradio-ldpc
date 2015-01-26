/*
 * This class gathers together all the test cases for the gnuradio-ldpc
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_ldpc.h"

CppUnit::TestSuite *
qa_ldpc::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("ldpc");

  return s;
}
