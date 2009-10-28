//*******************************************************************
//
// License:  See top level LICENSE.txt file.
//
// Author:  Jordi Inglada
//
// Description:
//
// Contains class definition for the class "ossimTileMapTileSource".
// ossimTileMapTileSource is derived from ImageHandler which is derived from
// TileSource.
//
//*******************************************************************
//  $Id: ossimTileMapTileSource.h 10751 2007-04-23 16:49:08Z dburken $

#ifndef ossimTileMapTileSource_HEADER
#define ossimTileMapTileSource_HEADER

#include <ossim/imaging/ossimGeneralRasterTileSource.h>
#include <ossim/base/ossimDate.h>
#include <ossim/base/ossimRefPtr.h>

class ossimFfL7;

class OSSIM_DLL ossimTileMapTileSource : public ossimGeneralRasterTileSource
{
public:
   ossimTileMapTileSource();

   ossimTileMapTileSource(const ossimKeywordlist& kwl,
                           const char* prefix=0);



   virtual ossimString getShortName() const;

   virtual ossimString getLongName() const;

   virtual ossimString  className() const;

   virtual bool open();

   virtual double getNullPixelValue(ossim_uint32 band=0)const;

   virtual double getMinPixelValue(ossim_uint32 band=0)const;

   virtual double getMaxPixelValue(ossim_uint32 band=0)const;

   virtual ossimScalarType getOutputScalarType() const;

   bool getAcquisitionDate(ossimDate& date)const;
   ossimString getSatelliteName()const;

   ossimFilename getBandFilename(ossim_uint32 idx)const;

   //! Returns the image geometry object associated with this tile source or NULL if non defined.
   //! The geometry contains full-to-local image transform as well as projection (image-to-world)
   virtual ossimImageGeometry* getImageGeometry();



   virtual bool loadState(const ossimKeywordlist& kwl,
                          const char* prefix = NULL);

   /**
    * @brief Gets a property for matching name.
    * @param name The name of the property to get.
    * @return Returns property matching "name".
    */
   virtual ossimRefPtr<ossimProperty> getProperty(const ossimString& name)const;

   /**
    * @brief Gets a list of property names available.
    * @param propertyNames The list to push back names to.
    */
   virtual void getPropertyNames(std::vector<ossimString>& propertyNames)const;

protected:
  virtual ~ossimTileMapTileSource();

private:

   void openHeader(const ossimFilename& file);

   ossimRefPtr<ossimFfL7> theFfHdr;

   TYPE_DATA
};

#endif
