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
#ifndef _MRFSamplerMAP_h
#define _MRFSamplerMAP_h

#include "otbMRFSampler.h"

namespace otb
{
/**
 * \class MRFSamplerMAP
 * \brief This is the base class for sampler methods used in the MRF framework.
 * 
 * This is one sampler to be used int he MRF framework. This sampler select the 
 * value which maximizes the apriori probability (minimum energy).
 */
  
template< class TInput1, class TInput2> 
class ITK_EXPORT MRFSamplerMAP : public MRFSampler< TInput1, TInput2 >
{
 public:
  
  typedef MRFSamplerMAP                  Self;
  typedef MRFSampler< TInput1, TInput2 > Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  
  typedef typename Superclass::InputImageNeighborhoodIterator    InputImageNeighborhoodIterator;
  typedef typename Superclass::LabelledImageNeighborhoodIterator LabelledImageNeighborhoodIterator;
  typedef typename Superclass::LabelledImagePixelType            LabelledImagePixelType;
  typedef typename Superclass::InputImagePixelType               InputImagePixelType;
  typedef typename Superclass::EnergyFidelityType                EnergyFidelityType;
  typedef typename Superclass::EnergyRegularizationType          EnergyRegularizationType;
  typedef typename Superclass::EnergyFidelityPointer             EnergyFidelityPointer;
  typedef typename Superclass::EnergyRegularizationPointer       EnergyRegularizationPointer;
  
   
  /*
  typedef itk::ConstNeighborhoodIterator< TInput1 >  InputImageNeighborhoodIterator;
  typedef itk::NeighborhoodIterator< TInput2 >  LabelledImageNeighborhoodIterator;
  typedef typename TInput2::PixelType LabelledImagePixelType;
  
  typedef MRFEnergy<TInput1, TInput2>  EnergyFidelityType;
  typedef MRFEnergy<TInput2, TInput2>  EnergyRegularizationType;
            
  typedef typename EnergyFidelityType::Pointer EnergyFidelityPointer;
  typedef typename EnergyRegularizationType::Pointer EnergyRegularizationPointer;
  */
  itkNewMacro(Self);
  
  itkTypeMacro(MRFSamplerMAP,MRFSampler);
  
            
  inline int Compute( const InputImageNeighborhoodIterator & itData, const LabelledImageNeighborhoodIterator & itRegul)             
    {
      this->SetEnergyBefore( this->GetEnergyFidelity()->GetValue(itData, itRegul.GetCenterPixel())
                             +  this->GetLambda() * this->GetEnergyRegularization()->GetValue(itRegul, itRegul.GetCenterPixel()) );
      
      //Try all possible value (how to be generic ?)
      this->SetEnergyAfter( this->GetEnergyBefore() ); //default values to current one
      this->SetValue( itRegul.GetCenterPixel() );
      for (LabelledImagePixelType valueCurrent = 0; valueCurrent< this->m_NumberOfClasses; ++valueCurrent)
	{
	  this->SetEnergyCurrent( this->GetEnergyFidelity()->GetValue(itData, valueCurrent)
				  + this->GetLambda() * this->GetEnergyRegularization()->GetValue(itRegul, valueCurrent) );  
	  if ( this->GetEnergyCurrent() < this->GetEnergyAfter() )
	    {
	      this->SetEnergyAfter( this->GetEnergyCurrent() );
	      this->SetValue( valueCurrent );
	    }
	  if (valueCurrent == itk::NumericTraits<LabelledImagePixelType>::max()) break;
	}
      
      this->SetDeltaEnergy( this->GetEnergyAfter() - this->GetEnergyBefore() );
      
      return 0;
    }
  
  
 protected:
  // The constructor and destructor.
  MRFSamplerMAP() {}
  virtual ~MRFSamplerMAP() {}
  
};
  
 
}

#endif
