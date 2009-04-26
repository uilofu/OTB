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
#ifndef __otbVectorDataKeywordlist_h
#define __otbVectorDataKeywordlist_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include <vector>
#include "ogr_feature.h"


namespace otb
{
/** \class VectorDataKeywordlist
   * \brief this class represents the metadata of vector data.
   *
 */

class VectorDataKeywordlist
{
  public:
    /** Smart pointer typedef support. */
    typedef VectorDataKeywordlist        Self;
//     typedef itk::LightObject               Superclass;
//     typedef itk::SmartPointer<Self>        Pointer;
//     typedef itk::SmartPointer<const Self>  ConstPointer;
//
//     /** Method for creation through the object factory. */
//     itkNewMacro(Self);
//
//     /** Run-time type information (and related methods). */
//     itkTypeMacro(VectorDataKeywordlist, LightObject);


    typedef std::vector<OGRFieldDefn> FieldListType;


    virtual void Print(std::ostream& os) const;

    void AddField(OGRFieldDefn* field)
    {
      //Fill up
    };

    VectorDataKeywordlist();
    virtual ~VectorDataKeywordlist();

    VectorDataKeywordlist(const Self&);
    void operator=(const Self&);

  protected:



  private:

    FieldListType m_FieldList;


};

}

#endif
