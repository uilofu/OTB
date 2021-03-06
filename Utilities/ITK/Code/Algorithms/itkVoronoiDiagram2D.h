/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVoronoiDiagram2D.h,v $
  Language:  C++
  Date:      $Date: 2009-04-23 03:53:37 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVoronoiDiagram2D_h
#define __itkVoronoiDiagram2D_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkCellInterface.h"
#include "itkLineCell.h"
#include "itkMesh.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkPolygonCell.h"
#include <vector>

#ifndef NULL
#define NULL 0 
#endif

namespace itk
{
/** \class VoronoiDiagram2D
 * \brief Implements the 2-Dimensional Voronoi Diagram.
 * 
 * Given a set of seed points, the Voronoi Diagram partitions the plane into
 * regions, each region is a collection of all pixels that is closest to one
 * particular seed point than to other seed points.
 * VoronoiDiagram2D is a mesh structure for storing the Voronoi Diagram, 
 * can be Generated by itkVoronoiDiagram2DGenerator.  
 *
 * Template parameters for VoronoiDiagram2D:
 *
 * TCoordType = The type associated with the coordniation of the seeds and
 * the resulting vertices.
 *
 * \ingroup MeshObjects 
 */
template <typename TCoordType>
class ITK_EXPORT VoronoiDiagram2D:
    public Mesh <TCoordType, 2,
                 DefaultDynamicMeshTraits<TCoordType, 2, 2, TCoordType> >
{
public:
  /** Standard class typedefs. */
  typedef VoronoiDiagram2D          Self;
  typedef Mesh <TCoordType, 2,
                DefaultDynamicMeshTraits<TCoordType, 2, 2, TCoordType> >
                                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Standard part of every itk Object. */
  itkTypeMacro(VoronoiDiagram2D, Mesh);

  /** Define the mesh traits. */
  typedef DefaultDynamicMeshTraits<TCoordType, 2, 2, TCoordType> MeshTraits;

  /** Dimensions of the points and topology. */
  itkStaticConstMacro(PointDimension, unsigned int,
                      MeshTraits::PointDimension);
  itkStaticConstMacro(MaxTopologicalDimension, unsigned int,
                      MeshTraits::MaxTopologicalDimension);

  /** Typedefs from itkMesh */
  typedef typename MeshTraits::PixelType               PixelType;  
  typedef typename MeshTraits::CoordRepType            CoordRepType;  
  typedef typename MeshTraits::InterpolationWeightType InterpolationWeightType;
  typedef typename MeshTraits::PointIdentifier         PointIdentifier;
  typedef typename MeshTraits::CellIdentifier          CellIdentifier;
  typedef typename MeshTraits::CellFeatureIdentifier   CellFeatureIdentifier;
  typedef typename MeshTraits::PointType               PointType;
  typedef typename MeshTraits::PointsContainer         PointsContainer;
  typedef typename MeshTraits::CellTraits              CellTraits;
  typedef typename MeshTraits::CellsContainer          CellsContainer;
  typedef typename MeshTraits::PointCellLinksContainer PointCellLinksContainer;
  typedef typename MeshTraits::CellLinksContainer      CellLinksContainer;
  typedef typename MeshTraits::PointDataContainer      PointDataContainer;
  typedef typename MeshTraits::CellDataContainer       CellDataContainer;  
  typedef typename MeshTraits::CellAutoPointer         genericCellPointer;
  typedef PointLocator<PointIdentifier,itkGetStaticConstMacro(PointDimension),
                       CoordRepType,PointsContainer>  PointLocatorType;
  typedef BoundingBox<PointIdentifier,itkGetStaticConstMacro(PointDimension),
                      CoordRepType,PointsContainer>   BoundingBoxType;
  typedef typename PointsContainer::Pointer       PointsContainerPointer;
  typedef typename CellsContainer::Pointer        CellsContainerPointer;
  typedef typename CellLinksContainer::Pointer    CellLinksContainerPointer;
  typedef typename PointDataContainer::Pointer    PointDataContainerPointer;
  typedef typename CellDataContainer::Pointer     CellDataContainerPointer;
  typedef typename PointLocatorType::Pointer      PointLocatorPointer;
  typedef typename BoundingBoxType::Pointer       BoundingBoxPointer;
  typedef typename
  PointsContainer::ConstIterator        PointsContainerConstIterator;
  typedef typename
  PointsContainer::Iterator             PointsContainerIterator;
  typedef typename
  CellsContainer::ConstIterator         CellsContainerConstIterator;
  typedef typename
  CellsContainer::Iterator              CellsContainerIterator;
  typedef typename
  CellLinksContainer::ConstIterator     CellLinksContainerIterator;
  typedef typename
  PointDataContainer::ConstIterator     PointDataContainerIterator;
  typedef typename
  CellDataContainer::ConstIterator      CellDataContainerIterator;
  typedef typename
  PointCellLinksContainer::const_iterator        PointCellLinksContainerIterator;

  typedef CellFeatureIdentifier                     CellFeatureCount;
  typedef CellInterface<PixelType,CellTraits>       CellInterfaceType;
  typedef PolygonCell<CellInterfaceType>            CellType;
  typedef typename CellType::CellAutoPointer        CellAutoPointer;
  typedef Point<int,2>                              EdgeInfo;
  typedef std::deque<EdgeInfo>                      EdgeInfoDQ;
  typedef typename CellType::MultiVisitor           CellMultiVisitorType;
  typedef std::vector<PointType>                    SeedsType;
  typedef typename SeedsType::iterator              SeedsIterator;
  typedef LineCell <CellInterfaceType>              Edge;
  typedef typename Edge::SelfAutoPointer            EdgeAutoPointer;
  typedef std::list<PointType>                      PointList;
  typedef std::vector<int>                          INTvector;
  typedef typename INTvector::iterator              NeighborIdIterator;
  typedef typename std::vector<PointType>::iterator VertexIterator;

  /** Get the number of Voronoi seeds. */
  itkGetConstMacro(NumberOfSeeds,unsigned int);
  
  /** Input the seeds information, will overwrite if seeds already
   * exists. */
  void SetSeeds (int num, SeedsIterator begin);

  /** The boundary that encloses the whole Voronoi diagram. */
  void SetBoundary(PointType vorsize);
  void SetOrigin(PointType vorsize);

  /** Iterators for the neighborhood cells around the given cell. */
  NeighborIdIterator NeighborIdsBegin(int seeds);
  NeighborIdIterator NeighborIdsEnd(int seeds);
  
  /** Iterators for all the vertices of the voronoi diagram. */ 
  VertexIterator VertexBegin(void);
  VertexIterator VertexEnd(void);
  
  /** Return the given indexed seed. */
  PointType GetSeed(int SeedID);

  /** Return the required cell pointer. */
  void GetCellId(CellIdentifier cellId, CellAutoPointer &);

  /** Return the given vertex of the voronoi diagram. */ 
  void GetPoint(int pId,PointType *answer);

  class VoronoiEdge{
  public:
    PointType m_Left;
    PointType m_Right;
    int       m_LeftID;
    int       m_RightID;
    int       m_LineID;
    VoronoiEdge(){}
    ~VoronoiEdge(){}
    };
  
  /** The iterator for Voronoi edges, */
  typedef typename std::vector<VoronoiEdge>::iterator VoronoiEdgeIterator;

  /** The Iterator of all the edges for the Voronoi diagram. */
  VoronoiEdgeIterator EdgeBegin(void);
  VoronoiEdgeIterator EdgeEnd(void);

  /** Find the two seed point that around the given edge. */
  EdgeInfo GetSeedsIDAroundEdge(VoronoiEdge *task);
  /********************************************************/

  void Reset();
  void InsertCells();

  void AddCellNeighbor(EdgeInfo x)
    { 
    m_CellNeighborsID[x[0]].push_back(x[1]);
    m_CellNeighborsID[x[1]].push_back(x[0]);
    }
  void ClearRegion(int i)
    { m_VoronoiRegions[i]->ClearPoints();}
  void VoronoiRegionAddPointId(int id, int x)
    { m_VoronoiRegions[id]->AddPointId(x);}
  void BuildEdge(int id)
    { m_VoronoiRegions[id]->BuildEdges();}

  void LineListClear(){ m_LineList.clear();}
  void EdgeListClear(){ m_EdgeList.clear();}
  void VertexListClear(){ m_VertexList.clear();}
  int LineListSize(){ return static_cast<int>( m_LineList.size() );} 
  int EdgeListSize(){ return static_cast<int>( m_EdgeList.size() );} 
  int VertexListSize(){ return static_cast<int>( m_VertexList.size() );} 
  void AddLine(EdgeInfo x){ m_LineList.push_back(x);}
  void AddEdge(VoronoiEdge x){ m_EdgeList.push_back(x);}
  void AddVert(PointType x){ m_VertexList.push_back(x);}
  EdgeInfo GetLine(int id){ return m_LineList[id];} 
  VoronoiEdge GetEdge(int id){ return m_EdgeList[id];} 
  PointType GetVertex(int id){ return m_VertexList[id];} 
  EdgeInfo GetEdgeEnd(int id)
    {
    EdgeInfo x;
    x[0]=m_EdgeList[id].m_LeftID;
    x[1]=m_EdgeList[id].m_RightID;
    return x;
    }
  int GetEdgeLineID(int id){ return m_EdgeList[id].m_LineID; }

protected:
  VoronoiDiagram2D();
  ~VoronoiDiagram2D();
  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  
  
private:
  VoronoiDiagram2D(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  SeedsType                       m_Seeds;
  unsigned int                    m_NumberOfSeeds;
  std::vector<CellType *>         m_VoronoiRegions;
  PointType                       m_VoronoiBoundary;
  PointType                       m_VoronoiBoundaryOrigin;
  std::vector< std::vector<int> > m_CellNeighborsID;
  
  std::vector< EdgeInfo >    m_LineList;
  std::vector< PointType >   m_VertexList;
  std::vector< VoronoiEdge > m_EdgeList;

}; // end class: VoronoiDiagram2D

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVoronoiDiagram2D.txx"
#endif

#endif
