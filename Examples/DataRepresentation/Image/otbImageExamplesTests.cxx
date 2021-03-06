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

// this file defines the otbMultiScaleTest for the test driver
// and all it expects is that you have a function called RegisterTests

#include <iostream>
#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(ImageAdaptor4Test);
  REGISTER_TEST(Image5Test);
  REGISTER_TEST(Image6Test);
}

#undef main
#define main ImageAdaptor4Test
#include "ImageAdaptor4.cxx"

#undef main
#define main Image5Test
#include "Image5.cxx"

#undef main
#define main Image6Test
#include "Image6.cxx"
