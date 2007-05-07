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
#ifndef _otbLinkPathFilter_h
#define _otbLinkPathFilter_h

#include "otbPathListToPathListFilter.h"
#include "otbMacro.h"

namespace otb
{
/** \class LinkPathFilter
 *  \brief This filters link path from the input path list according to distance and angular criterions.
 *  
 *  This filter examines both ends of each path to link and is able to link several path into one single path.
 *  The two criterions to link a path are :
 *  - The two ends to link must be closer than the user defined threshold in terms of euclidean distance,
 *  - The angle at the link must not be sharp (less than the user defined threshold).
 *  
 *  Please note that this filter may invert the order of the vertices in the newly created path.
 *
 *  This filter is part of the road extraction framework.
 */
template <class TPath>
class ITK_EXPORT LinkPathFilter
  : public PathListToPathListFilter<TPath>
{
 public:
  /** Standard typedefs */
  typedef LinkPathFilter            Self;
  typedef PathListToPathListFilter<TPath>       Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;
  
  /** Type macro */
  itkNewMacro(Self);
  
  /** Creation through object factory macro */
  itkTypeMacro(LinkPathFilter, PathListToPathListFilter);
  
  /** Template parameters typedefs */
  typedef typename Superclass::PathType         PathType;
  typedef typename Superclass::PathListType     PathListType;
  typedef typename Superclass::PathPointerType  PathPointerType;
  typedef typename PathListType::Pointer        PathListPointerType;
  typedef typename PathListType::ConstIterator IteratorType;
  typedef typename PathType::VertexType VertexType;
  typedef typename PathType::VertexListType VertexListType;
  typedef typename VertexListType::ConstIterator VertexIteratorType;
  typedef double 								RealType;
  
  itkSetMacro(AngularThreshold,RealType);
  itkGetMacro(AngularThreshold,RealType);
  itkSetMacro(DistanceThreshold,RealType);
  itkGetMacro(DistanceThreshold,RealType);

protected:
  /** Constructor */
  LinkPathFilter();
  /** Destructor */
  virtual ~LinkPathFilter() {};
  /** GenerateData method */
  virtual void GenerateData();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /**
   * Verify the angular condition to link a path.
   * \param v1 First vertex (before first path ending),
   * \param v2 second vertex (first path ending),
   * \param v3 third vertex (second path beginning),
   * \param v4 fourth vertex (second path after beginning),
   * \return True or false wether the condition is verified or not.
   **/
  bool VerifyAngularCondition(VertexType v1, VertexType v2, VertexType v3, VertexType v4);

  /**
   * Actually link the end of path p1 to the beginning of path p2.
   * \param p1 First path to link,
   * \param revert1 wether p1 need to be reverted or not,
   * \param p2 second path to link,
   * \param revert2 wether p2 need to be reverted or not,
   * \return the linked path.
   */
  PathPointerType LinkPath(PathPointerType p1,bool revert1, PathPointerType p2, bool revert2);

private:
  LinkPathFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RealType m_AngularThreshold;
  RealType m_DistanceThreshold;
  
};
}// End namespace otb
#ifndef OTB_MANUAL_INSTANTIATION
#include "otbLinkPathFilter.txx"
#endif

#endif
