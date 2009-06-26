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

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkExceptionObject.h"

#include <fstream>
#include <iostream>
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageMetadataInterface.h"


int otbImageMetadataInterfaceTest (int argc, char* argv[])
{
  // Verify the number of parameters in the command line
  const char * inputFilename  = argv[1];
  const char * outputFilename  = argv[2];

  typedef otb::VectorImage< double,  2 >                 InputImageType;
  typedef otb::ImageFileReader< InputImageType >         ImageReaderType;

  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputFilename  );
  reader->UpdateOutputInformation();
  otb::ImageMetadataInterface::Pointer lImageMetadata = otb::ImageMetadataInterface::New();

  std::ofstream file;
  file.open(outputFilename);

  file<<"GetSensorID:        "<<lImageMetadata->GetSensorID(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetPhysicalGain:    "<<lImageMetadata->GetPhysicalGain(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetPhysicalBias:    "<<lImageMetadata->GetPhysicalBias(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetDay:             "<<lImageMetadata->GetDay(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetMonth:           "<<lImageMetadata->GetMonth(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetYear:            "<<lImageMetadata->GetYear(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetSolarIrradiance: "<<lImageMetadata->GetSolarIrradiance(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetSunElevation:    "<<lImageMetadata->GetSunElevation(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file<<"GetSunAzimuth:    "<<lImageMetadata->GetSunAzimuth(reader->GetOutput()->GetMetaDataDictionary())<<std::endl;
  file.close();

  return EXIT_SUCCESS;

}