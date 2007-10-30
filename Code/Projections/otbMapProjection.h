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
#ifndef __otbMapProjection_h
#define __otbMapProjection_h

#include <iostream>
#include <sstream>
#include <stdio.h>
#include "projection/ossimMapProjection.h" 
#include "itkTransform.h"
#include "itkExceptionObject.h"
#include "itkMacro.h"
#include "base/ossimGpt.h"
#include "base/ossimDpt.h"
#include "projection/ossimProjection.h"
#include "base/ossimEllipsoid.h"
#include "base/ossimEllipsoidFactory.h"
#include "base/ossimString.h"


namespace otb
{
  
  /** \class otbMapProjection
   *  \brief This is the base class for all geographic projections (UTM, Lambert, ...)
   **/
  
  typedef enum {FORWARD=0, INVERSE=1} InverseOrForwardTransformationEnum;
  
  template <class TOssimMapProjection,
						InverseOrForwardTransformationEnum transform,
            class TScalarType = double,
            unsigned int NInputDimensions=2,
            unsigned int NOutputDimensions=2>
    class ITK_EXPORT MapProjection: public itk::Transform<TScalarType,       // Data type for scalars 
                                                          NInputDimensions,  // Number of dimensions in the input space
                                                          NOutputDimensions> // Number of dimensions in the output space
    {
      public :
      /** Standard class typedefs. */
      typedef itk::Transform< TScalarType,
                              NInputDimensions,
                              NOutputDimensions >       Superclass;
      typedef MapProjection                    	        Self;
      typedef itk::SmartPointer<Self>                   Pointer;
      typedef itk::SmartPointer<const Self>             ConstPointer;
      
      typedef typename Superclass::ScalarType 	        ScalarType;
      typedef TOssimMapProjection  			OssimMapProjectionType;
      typedef itk::Point<ScalarType,NInputDimensions >  InputPointType;
      typedef itk::Point<ScalarType,NOutputDimensions > OutputPointType;        
      
      /** Method for creation through the object factory. */
      itkNewMacro( Self );
      
      /** Run-time type information (and related methods). */
      itkTypeMacro( MapProjection, Transform );
      
      typedef InverseOrForwardTransformationEnum DirectionOfMappingEnumType;
      
      itkStaticConstMacro(DirectionOfMapping,DirectionOfMappingEnumType,transform);
      itkStaticConstMacro(InputSpaceDimension, unsigned int, NInputDimensions);
      itkStaticConstMacro(OutputSpaceDimension, unsigned int, NOutputDimensions);
      itkStaticConstMacro(SpaceDimension, unsigned int, NInputDimensions);
      itkStaticConstMacro(ParametersDimension, unsigned int,NInputDimensions*(NInputDimensions+1));
      
      
      virtual void SetEllipsoid ();
      void SetEllipsoid (const ossimEllipsoid &ellipsoid);
      void SetEllipsoid(std::string code);
      void SetEllipsoid(const double &major_axis, const double &minor_axis);
      //OutputPointType Forward(const InputPointType &point) const;
      //InputPointType Inverse(const OutputPointType &point) const;
	
      OutputPointType TransformPoint(const InputPointType &point) const;
      virtual InputPointType Origin();
      //virtual void LineSampleToEastingNorthing(const InputPointType &linesample, InputPointType &eastingNorthing) const; 
      virtual double GetFalseNorthing() const;
      virtual double GetFalseEasting() const;
      virtual double GetStandardParallel1() const;
      virtual double GetStandardParallel2() const;
      virtual std::string GetProjectionName() const;
      virtual bool IsGeographic() const; 
      virtual double GetA() const;
      virtual double GetB() const;
      virtual double GetF() const;
      virtual OutputPointType GetMetersPerPixel() const;
      virtual OutputPointType GetDecimalDegreesPerPixel() const;
      virtual void SetAB(double a, double b);
      virtual void SetOrigin(const InputPointType &origin);
      virtual void SetMetersPerPixel(const OutputPointType &point);
      virtual void SetDecimalDegreesPerPixel(const OutputPointType &point);
      virtual void ComputeDegreesPerPixel(const InputPointType &ground, const OutputPointType &metersPerPixel, double &deltaLat, double &deltaLon);
      virtual void ComputeMetersPerPixel(const InputPointType &center, double deltaDegreesPerPixelLat, double deltaDegreesPerPixelLon, OutputPointType &metersPerPixel);
      virtual void ComputeMetersPerPixel(double deltaDegreesPerPixelLat, double deltaDegreesPerPixelLon, OutputPointType &metersPerPixel);
      //virtual void SetMatrix(double rotation,  const OutputPointType &scale, const OutputPointType &translation);
      void SetFalseEasting(double falseEasting);
			
			/** Return the inverse of the transform.
   		 *  The inverse is recomputed if it has been modified */
		  //bool GetInverse(Self*) const ;

      protected:
      MapProjection();
      virtual ~MapProjection();
      OssimMapProjectionType* m_MapProjection;  
      
      private :
      MapProjection(const Self&); //purposely not implemented
      void operator=(const Self&); //purposely not implemented
    };
  
} // namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbMapProjection.txx"
#endif

#endif
