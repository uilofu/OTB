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



#include "itkExceptionObject.h"

#include <iostream>

#include "otbSpotImageMetadataInterface.h"

int otbSpotImageMetadataInterfaceNew(int argc, char* argv[])
{
  otb::SpotImageMetadataInterface::Pointer object = otb::SpotImageMetadataInterface::New();

  std::cout << object << std::endl;

  return EXIT_SUCCESS;
}
