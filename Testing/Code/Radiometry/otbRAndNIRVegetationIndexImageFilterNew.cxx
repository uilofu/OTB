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

#include "otbRAndNIRIndexImageFilter.h"
#include "itkImage.h"
#include "otbImage.h"

int otbRAndNIRVegetationIndexImageFilterNew(int argc, char * argv[])
{
  const unsigned int Dimension = 2;
  typedef unsigned char                    PixelType;
  typedef otb::Image<PixelType, Dimension> InputRImageType;
  typedef itk::Image<PixelType, Dimension> InputNIRImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  typedef otb::RAndNIRIndexImageFilter<InputRImageType, InputNIRImageType, OutputImageType> RAndNIRIndexImageFilterType;

  // Instantiating object
  RAndNIRIndexImageFilterType::Pointer filter = RAndNIRIndexImageFilterType::New();

  std::cout << filter << std::endl;

  return EXIT_SUCCESS;
}
