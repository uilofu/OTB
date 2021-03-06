//*******************************************************************
///
// License:  LGPL
// 
// See LICENSE.txt file in the top level directory for more details.
// 
// Author: Garrett Potts
//
//*************************************************************************
// $Id: ossimPluginLibrary.cpp 16098 2009-12-15 15:13:16Z dburken $
#include <ossim/plugin/ossimPluginLibrary.h>

RTTI_DEF1(ossimPluginLibrary, "ossimPluginLibrary", ossimDynamicLibrary);

ossimPluginLibrary::ossimPluginLibrary()
   :ossimDynamicLibrary(),
    theInfo(0)
{
}

ossimPluginLibrary::ossimPluginLibrary(const ossimString& name)
   :ossimDynamicLibrary(name),
    theInfo(0)
{
   initialize();
}

ossimPluginLibrary::~ossimPluginLibrary()
{
   finalize();
}

void ossimPluginLibrary::initialize()
{
   if(!isLoaded())
   {
      load();
   }

   ossimSharedLibraryInitializePtr init = (ossimSharedLibraryInitializePtr)getSymbol("ossimSharedLibraryInitialize");
   
   if(init)
   {         
      init(&theInfo);
   }
}

void ossimPluginLibrary::finalize()
{
   ossimSharedLibraryInitializePtr finalizeLib = (ossimSharedLibraryInitializePtr)getSymbol("ossimSharedLibraryFinalize");
   if(finalizeLib)
   {         
      finalizeLib(&theInfo);
   }
   
   unload();
}

ossimString ossimPluginLibrary::getDescription()const
{
   ossimString result;

   if(theInfo&&isLoaded()&&theInfo->getDescription)
   {
      result = theInfo->getDescription();
   }

   return result;
}

void ossimPluginLibrary::getClassNames(std::vector<ossimString>& classNames)const
{
   if(theInfo&&theInfo->getNumberOfClassNames&&theInfo->getClassName)
   {
      ossim_int32 idx        = 0;
      ossim_int32 numObjects = 0;

      numObjects = theInfo->getNumberOfClassNames();
      for(idx = 0; idx < numObjects; ++idx)
      {
         classNames.push_back(theInfo->getClassName(idx));
      }
   }
}
