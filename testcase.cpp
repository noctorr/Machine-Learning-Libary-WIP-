#include "./SRC/nex.hpp"

int main()
{
  // ADDITION TEST CASES
  IEE_754_FLOAT testcase1_double_0(3.0);
  IEE_754_FLOAT testcase1_double_1(2.0);

  double output = testcase1_double_1 + testcase1_double_0;
  // 5.0 (PASSED)

  IEE_754_FLOAT testcase2_double_2(3.5);
  IEE_754_FLOAT testcase2_double_3(4.6);

  double output_2 = testcase2_double_2 + testcase2_double_3;
  // ABLE TO INTERPRET 3.5 FLOAT HOWEVER WHEN IT COMES TO IRREGULAR FLOATS LIKE 4.6 WHERE IT CANNOT BE GIVEN AN EXACT VALUE IN BINARY FORM IT CANNOT INTERPRET 4.6 PROPERLY (TRYING HARD TO FIX IT)
  // FAIL
  return 0;
}
