/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiphaseDenseFiniteDifferenceImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2010-02-25 14:19:24 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMultiphaseDenseFiniteDifferenceImageFilter_txx
#define __itkMultiphaseDenseFiniteDifferenceImageFilter_txx

#include "itkMultiphaseDenseFiniteDifferenceImageFilter.h"

namespace itk
{

template < class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void 
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "m_ReinitializeCounter: " <<m_ReinitializeCounter <<std::endl;
}

template < class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::CopyInputToOutput()
{
  OutputImagePointer output = this->GetOutput();
  output->FillBuffer( 0 );

  for( IdCellType i = 0; i < this->m_FunctionCount; i++ )
    {
    const InputImagePointer input = this->m_LevelSet[i];
    const InputPointType origin = input->GetOrigin();
    const InputSizeType size = input->GetBufferedRegion().GetSize();

    // Find the index of the target image where this Level Set
    // should be pasted.
    OutputIndexType start;
    output->TransformPhysicalPointToIndex( origin, start );

    OutputRegionType region;
    region.SetSize( size );
    region.SetIndex( start );

    if ( !input || !output )
      {
      itkExceptionMacro ( << "Either input and/or output is NULL." );
      }

    ImageRegionConstIterator< InputImageType > in( input, input->GetBufferedRegion() );
    ImageRegionIterator< OutputImageType > out( output, region );

    // Fill the output pointer
    OutputPixelType p = static_cast<OutputPixelType> ( this->m_Lookup[i] );

    in.GoToBegin();
    out.GoToBegin();

    while( !out.IsAtEnd() )
      {
      if( in.Get() < 0 )
        {
        out.Value() =  p;
        }
      ++in;
      ++out;
      }
  }
}

template < class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::AllocateUpdateBuffer()
{
  for( IdCellType i = 0; i < this->m_FunctionCount; i++ )
    {
    InputImagePointer input = this->m_LevelSet[i];
    InputRegionType region = input->GetLargestPossibleRegion();

    m_UpdateBuffers[i]->CopyInformation( input );
    m_UpdateBuffers[i]->SetRegions( region );
    m_UpdateBuffers[i]->Allocate();
    }
}

template < class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
typename MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >::TimeStepType
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::CalculateChange()
{
  TimeStepType timeStep = NumericTraits< TimeStepType >::max();

  for( IdCellType i = 0; i < this->m_FunctionCount; i++ )
    {
    InputImagePointer levelset = this->m_LevelSet[i];

    // Get the FiniteDifferenceFunction to use in calculations.
    const FiniteDifferenceFunctionPointer df = this->m_DifferenceFunctions[i];

    const OutputSizeType radius = df->GetRadius();

    // Break the input into a series of regions.  The first region is free
    // of boundary conditions, the rest with boundary conditions.  We operate
    // on the levelset region because input has been copied to output.
    FaceCalculatorType faceCalculator;
    FaceListType faceList =
      faceCalculator ( levelset, levelset->GetLargestPossibleRegion(), radius );

    void *globalData;

    // Ask the function object for a pointer to a data structure it
    // will use to manage any global values it needs.  We'll pass this
    // back to the function object at each calculation and then
    // again so that the function object can use it to determine a
    // time step for this iteration.
    globalData = df->GetGlobalDataPointer();

    typename FaceListType::iterator fIt;
    for ( fIt = faceList.begin(); fIt != faceList.end(); ++fIt )
      {
      // Process the non-boundary region.
      NeighborhoodIteratorType nD ( radius, levelset, *fIt );
      ImageRegionIterator< InputImageType > nU ( m_UpdateBuffers[i], *fIt );

      nD.GoToBegin();
      nU.GoToBegin();

      while( !nD.IsAtEnd() )
        {
        nU.Value() = df->ComputeUpdate ( nD, globalData );
        ++nD;
        ++nU;
        }
      }

    // Ask the finite difference function to compute the time step for
    // this iteration.  We give it the global data pointer to use, then
    // ask it to free the global data memory.
    TimeStepType dt = df->ComputeGlobalTimeStep ( globalData );
    df->ReleaseGlobalDataPointer ( globalData );

    if ( dt < timeStep )
      {
      timeStep = dt;
      }
    }

  timeStep = 0.08;  // FIXME !!! After all this work, assign a constant !!! Why ??

  return timeStep;
}

template< class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::SetFunctionCount( const IdCellType& n )
{
  this->Superclass::SetFunctionCount( n );

  this->m_UpdateBuffers.resize( n, 0 );

  for( IdCellType i = 0; i < this->m_FunctionCount; i++ )
    {
    this->m_UpdateBuffers[i] = InputImageType::New();
    }
}

template< class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::ApplyUpdate ( TimeStepType dt )
{
  double rms_change_accumulator = 0;
  double den = 0;
  IdCellType i;
  InputPixelType val;

  for( i = 0;  i < this->m_FunctionCount; i++ )
    {
    const double img_size = this->m_LevelSet[i]->GetLargestPossibleRegion().GetNumberOfPixels();
    den += img_size;
    }

  // this must never occur!
  if( den < vnl_math::eps )
    {
    itkExceptionMacro( "den = 0." );
    }

  // Updating the output image
  for ( i = 0;  i < this->m_FunctionCount; i++ )
    {
    //NOTE: here this->m_LevelSet[i]->GetRequestedRegion() is used and previously
    // it is this->m_LevelSet[i]->GetLargestPossibleRegion()
    InputRegionType region = this->m_LevelSet[i]->GetRequestedRegion();

    ImageRegionIterator< InputImageType > u( m_UpdateBuffers[i], region );
    ImageRegionIterator< InputImageType >  o( this->m_LevelSet[i], region );

    u.GoToBegin();
    o.GoToBegin();

    while( !u.IsAtEnd() )
      {
      val = static_cast< InputPixelType > ( dt ) * u.Get();
      o.Set( o.Value() + val );
      rms_change_accumulator += static_cast<double> ( vnl_math_sqr( val ) );
      ++u;
      ++o;
      }

    if ( this->GetElapsedIterations() % this->m_ReinitializeCounter == 0 )
      {
      ThresholdFilterPointer thresh = ThresholdFilterType::New();
      thresh->SetLowerThreshold( NumericTraits< InputPixelType >::NonpositiveMin() );
      thresh->SetUpperThreshold( 0 );
      thresh->SetInsideValue( 1 );
      thresh->SetOutsideValue( 0 );
      thresh->SetInput( this->m_LevelSet[i] );
      thresh->Update();

      MaurerPointer maurer = MaurerType::New();
      maurer->SetInput( thresh->GetOutput() );
      maurer->SetSquaredDistance( 0 );
      maurer->SetUseImageSpacing( this->m_UseImageSpacing );
      maurer->SetInsideIsPositive( 0 );
      maurer->Update();

      ImageRegionIterator< InputImageType >  it ( maurer->GetOutput(), region );

      rms_change_accumulator = 0;
      
      o.GoToBegin();
      it.GoToBegin();

      while( !o.IsAtEnd() )
        {
        val = it.Value();
        rms_change_accumulator += static_cast<double> ( vnl_math_sqr( o.Value() - val ) );
        o.Set( val );
        ++o;
        ++it;
        }
      }
    }

  this->SetRMSChange( vcl_sqrt(rms_change_accumulator / den ) );
}

template< class TInputImage, class TFeatureImage, class TOutputImage,
  class TFunction, typename TIdCell >
void
MultiphaseDenseFiniteDifferenceImageFilter< TInputImage, TFeatureImage,
  TOutputImage, TFunction, TIdCell >
::PostProcessOutput()
{
  this->CopyInputToOutput();
}

}// end namespace itk

#endif
