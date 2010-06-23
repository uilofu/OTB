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

#include "otbScalarImageToTexturesFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbStandardFilterWatcher.h"

int otbScalarImageToTexturesFilter(int argc, char * argv[])
{
  if(argc != 7)
    {
    std::cerr<<"Usage: "<<argv[0]<<" infname outprefix nbBins radius offsetx offsety"<<std::endl;
    return EXIT_FAILURE;
    }
  const char * infname      = argv[1];
  const char * outprefix    = argv[2];
  const unsigned int nbBins = atoi(argv[3]);
  const unsigned int radius = atoi(argv[4]);
  const int offsetx         = atoi(argv[5]);
  const int offsety         = atoi(argv[6]);

  const unsigned int Dimension = 2;
  typedef float                                  PixelType;
  typedef otb::Image<PixelType, Dimension>       ImageType;
  typedef otb::ScalarImageToTexturesFilter
    <ImageType,ImageType>                        TexturesFilterType;
  typedef otb::ImageFileReader<ImageType>        ReaderType;
  typedef otb::ImageFileWriter<ImageType>        WriterType;
  typedef itk::MinimumMaximumImageCalculator
    <ImageType>                                  MinMaxCalculatorType;

  ReaderType::Pointer         reader = ReaderType::New();
  TexturesFilterType::Pointer filter = TexturesFilterType::New();
  WriterType::Pointer         writer = WriterType::New();

  // Read image
  reader->SetFileName(infname);
  reader->Update();

  // Build radius
  TexturesFilterType::SizeType sradius;
  sradius.Fill(radius);

  // Build offset
  TexturesFilterType::OffsetType offset;
  offset[0] = offsetx;
  offset[1] = offsety;

  filter->SetInput(reader->GetOutput());
  filter->SetNumberOfBinsPerAxis(nbBins);
  filter->SetRadius(sradius);
  filter->SetOffset(offset);

  otb::StandardFilterWatcher watcher(filter,"Textures filter");

  // Compute min/max
  MinMaxCalculatorType::Pointer minMax = MinMaxCalculatorType::New();
  minMax->SetImage(reader->GetOutput());
  minMax->Compute();

  filter->SetInputImageMinimum(minMax->GetMinimum());
  filter->SetInputImageMaximum(minMax->GetMaximum());
  filter->Update();

  // Write outputs
  itk::OStringStream oss;

  oss.str("");
  oss<<outprefix<<"Energy.tif";
  writer->SetInput(filter->GetEnergyOutput());
  writer->SetFileName(oss.str());

  oss.str("");
  oss<<outprefix<<"Entropy.tif";
  writer->SetInput(filter->GetEntropyOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"Correlation.tif";
  writer->SetInput(filter->GetCorrelationOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"InverseDifferenceMoment.tif";
  writer->SetInput(filter->GetInverseDifferenceMomentOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"Inertia.tif";
  writer->SetInput(filter->GetInertiaOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"ClusterShade.tif";
  writer->SetInput(filter->GetClusterShadeOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"ClusterProminence.tif";
  writer->SetInput(filter->GetClusterProminenceOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  oss.str("");
  oss<<outprefix<<"HaralickCorrelation.tif";
  writer->SetInput(filter->GetHaralickCorrelationOutput());
  writer->SetFileName(oss.str());
  writer->Update();

  return EXIT_SUCCESS;
}