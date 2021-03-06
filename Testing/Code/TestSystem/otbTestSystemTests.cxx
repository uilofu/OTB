/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// this file defines the otbGISFiltersTest for the test driver
// and all it expects is that you have a function called RegisterTests


#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbIsNightlyRevision);
  REGISTER_TEST(otbCompareAsciiTests);
  REGISTER_TEST(otbCompareAsciiTests2);
  REGISTER_TEST(otbCompareAsciiTests3);
  REGISTER_TEST(otbCompareAsciiTests4);
  REGISTER_TEST(otbCompareAsciiTests5);
  REGISTER_TEST(otbCompareAsciiTestsEpsilon3_WhiteSpace);
  REGISTER_TEST(otbCompareAsciiTestScientificNotation);
}
