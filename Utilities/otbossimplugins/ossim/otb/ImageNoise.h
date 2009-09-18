//----------------------------------------------------------------------------
//
// "Copyright Centre National d'Etudes Spatiales"
//
// License:  LGPL
//
// See LICENSE.txt file in the top level directory for more details.
//
//----------------------------------------------------------------------------
// $Id$

#ifndef ImageNoise_h
#define ImageNoise_h

#include <vector>
#include <ossim/base/ossimString.h>

class ossimKeywordlist;

namespace ossimplugins
{


/**
 * @ingroup ImageNoise
 * @brief This class represents an ImageNoise
 */
class ImageNoise
{
public:
   /**
    * @brief Constructor
    */
   ImageNoise();

   /**
    * @brief Destructor
    */
   virtual ~ImageNoise();

   /**
    * @brief Copy constructor
    */
   ImageNoise(const ImageNoise& rhs);

   /**
    * @brief Affectation operator
    */
   ImageNoise& operator=(const ImageNoise& rhs);

   /**
    * @brief Method to save object state to a keyword list.
    * @param kwl Keyword list to save to.
    * @param prefix added to keys when saved.
    * @return true on success, false on error.
    */
   bool saveState(ossimKeywordlist& kwl, const char* prefix=0) const;

   /**
    * @brief Method to the load (recreate) the state of the object from a
    * keyword list. Return true if ok or false on error.
    * @return true if load OK, false on error
    */
   bool loadState (const ossimKeywordlist &kwl, const char *prefix=0);

   void set_timeUTC(const ossimString& value)
   {
      _timeUTC = value;
   }
   void set_validityRangeMin(double value)
   {
      _validityRangeMin = value;
   }
   void set_validityRangeMax(double value)
   {
      _validityRangeMax = value;
   }
   void set_referencePoint(double value)
   {
      _referencePoint = value;
   }
   void set_polynomialDegree(unsigned int value)
   {
      _polynomialDegree = value;
   }
   void set_polynomialCoefficients(const std::vector<double>& value)
   {
      _polynomialCoefficients = value;
   }

protected:

      /**
       * @brief TimeUTC (Noise/ImageNoise node).
       */
      ossimString _timeUTC;

      /**
       * @brief ValidityRangeMin (Noise/ImageNoise node).
       */
      double _validityRangeMin;
      /**
       * @brief ValidityRangeMax (Noise/ImageNoise node).
       */
      double _validityRangeMax;
      /**
       * @brief Reference point (Noise/ImageNoise node).
       */
      double _referencePoint;

      /**
       * @brief Polynomial degree (Noise/ImageNoise node).
       */
      unsigned int _polynomialDegree;
      /**
       * @brief Polynomial coefficients (Noise/ImageNoise node).
       */
      std::vector<double> _polynomialCoefficients;

private:
};
}

#endif
