// this file defines the otbCommonTest for the test driver
// and all it expects is that you have a function called RegisterTests
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#define MAIN

#include <iostream>
#include "otbTestMain.h" 

void RegisterTests()
{
REGISTER_TEST(otbLeeFilter);
REGISTER_TEST(otbFrostFilter);
REGISTER_TEST(otbAlignImageToPath);
REGISTER_TEST(otbFlstTest);
}
