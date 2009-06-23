/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Some parts of this code are derived from ITK. See ITKCopyright.txt
  for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbTestHelper.h"
#include "otbMacro.h"
#include <iostream>
#include <fstream>


#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbDifferenceImageFilter.h"
#include "otbPrintableImageFilter.h"

#define ITK_TEST_DIMENSION_MAX 6

// OGR Headers files
#include "cpl_conv.h"
#include "cpl_string.h"
#include "cpl_multiproc.h"

#define otbPrintDiff(comment,refStr,testStr) \
        std::cout << "   ----    '"<<comment<<"' checking   ---------------------------"<<std::endl; \
        std::cout << "   Base << "<<refStr<<std::endl; \
        std::cout << "   Test >> "<<testStr<<std::endl;

#define otbCheckStringValue(Comment,RefStr,TestStr, count, report) \
{ \
        std::string strRefStr(""); if(RefStr != NULL) strRefStr = RefStr; \
        std::string strTestStr(""); if(TestStr != NULL) strTestStr = TestStr; \
        if( strRefStr != strTestStr ) \
        { \
            count++; \
            if(report) \
            { \
                otbPrintDiff(Comment,strRefStr,strTestStr); \
            } \
        } \
}
#define otbCheckValue(Comment,Ref,Test, count, report) \
{ \
        if( Ref != Test ) \
        { \
            count++; \
            if(report) \
            { \
                otbPrintDiff(Comment,Ref,Test); \
            } \
        } \
}

namespace otb
{


  int TestHelper::RegressionTestAsciiFile(const char * testAsciiFileName, const char * baselineAsciiFileName, int reportErrors, const double epsilon, std::vector<std::string> ignoredLines) const
  {
    std::ifstream fluxfiletest(testAsciiFileName);
    std::ifstream fluxfileref(baselineAsciiFileName);
    enum TypeEtat { ETAT_NUM, ETAT_CHAR };

    std::string diffAsciiFileName(testAsciiFileName);
    diffAsciiFileName += ".diff.txt";
    std::ofstream fluxfilediff;
    if ( reportErrors )
    {
      fluxfilediff.open(diffAsciiFileName.c_str());
    }

    std::string strfiletest;
    std::string strfileref;

    int nbdiff(0);
    int numLine(1);
    if (!fluxfiletest)
    {
      itkGenericExceptionMacro(<<"Impossible to open the test ASCII file <"<<testAsciiFileName<<">.");
    }
    if (!fluxfileref)
    {
      itkGenericExceptionMacro(<< "Impossible to open the baseline ASCII file <"<<baselineAsciiFileName<<">.");
    }

    TypeEtat etatPrec(ETAT_NUM), etatCour(ETAT_NUM);

    std::vector<std::string> listStrDiffLineFileRef;
    std::vector<std::string> listStrDiffLineFileTest;



    while ( std::getline(fluxfileref,strfileref)!=0  )
    {
      otb::StringStream buffstreamRef, buffstreamTest;
      buffstreamRef << strfileref;

    //check if we've reach end of test file
      if (std::getline(fluxfiletest,strfiletest) == 0)
      {
        std::string strRef = "";

        buffstreamRef >> strRef;
        fluxfilediff << "Line missing in test file: " << numLine
            << " : " << strRef
            << std::endl;
        nbdiff++;
      }
      else
      {

        bool foundexpr = false;
        if (ignoredLines.size()>0)
        {

          std::vector<std::string>::iterator itIgnoredLines = ignoredLines.begin();

          for (;(itIgnoredLines != ignoredLines.end()); ++itIgnoredLines)
          {
            std::string ignoredLinesAscii = (*itIgnoredLines);
            std::string::size_type loc = strfileref.find(ignoredLinesAscii);
            if ( loc != std::string::npos )
            {
              foundexpr = true;
            }

          }

        }

        if ( foundexpr == false )
        {
          buffstreamTest << strfiletest;
          int nblinediff(0);

          while (buffstreamRef.peek() != EOF)
          {
            std::string strRef = "";
            std::string strTest = "";

            std::string strNumRef = "";
            std::string strCharRef = "";
            std::string strNumTest = "";
            std::string strCharTest = "";

            buffstreamRef >> strRef;
            buffstreamTest >> strTest;

            bool chgt= false;
            std::string charTmpRef = "";
            std::string charTmpTest = "";
            unsigned int i=0;
            if (!isHexaPointerAddress(strRef))
            {
            //Analyse if strRef contains scientific value (ex: "-142.124e-012")
              if (isScientificNumeric(strRef))
              {
                if (!isScientificNumeric(strTest))
                {
                  if ( reportErrors )
                  {
                    fluxfilediff << "Diff at line " << numLine << " : compare numeric value with no numeric value ("<<strRef
                        << strRef << " != " << strTest <<")"<< std::endl;
                    nblinediff++;
                  }
                  nbdiff++;

                }
                else if ( (strRef != strTest)
                           && (vcl_abs(atof(strRef.c_str())) > m_EpsilonBoundaryChecking)
                           && (vcl_abs(atof(strRef.c_str())-atof(strTest.c_str()))
                           > epsilon*vcl_abs(atof(strRef.c_str()))
                              ) )//epsilon as relative error
                {
                  if ( reportErrors )
                  {
                    fluxfilediff << "Diff at line " << numLine << " : vcl_abs ( ("
                        << strRef << ") - (" << strTest
                        << ") ) > " << epsilon << std::endl;
                    nblinediff++;
                  }
                  nbdiff++;
                }
              }
              else
              {
                while (i < strRef.size())
                {
                  charTmpRef=strRef[i];

                  if (i<strTest.size())
                  {
                    charTmpTest=strTest[i];
                  }

                  if (isNumeric(charTmpRef))
                    etatCour = ETAT_NUM;
                  else
                    etatCour = ETAT_CHAR;

                // "reference" state initialisation.
                  if (i==0)
                    etatPrec=etatCour;

                // Case where there's a number after characteres.
                  if ((etatCour==ETAT_NUM)&&(etatPrec==ETAT_CHAR))
                  {
                    if ( strCharRef != strCharTest )
                    {
                      if ( reportErrors )
                      {
                        fluxfilediff << "Diff at line " << numLine
                            << " : " << strCharRef
                            << " != " << strCharTest << std::endl;
                        nblinediff++;
                      }
                      nbdiff++;
                    }

                    strCharRef="";
                    strCharTest="";
                    strNumRef=charTmpRef;
                    strNumTest=charTmpTest;
                    chgt=true;
                  }
                // Case where there's a character after numbers.
                  else if ((etatCour==ETAT_CHAR)&&(etatPrec==ETAT_NUM))
                  {

                    if ( (strNumRef != strNumTest)
                          && (vcl_abs(atof(strNumRef.c_str())) > m_EpsilonBoundaryChecking)
                          && (vcl_abs(atof(strNumRef.c_str())-atof(strNumTest.c_str()))
                          > epsilon*vcl_abs(atof(strNumRef.c_str()))
                             ) ) //epsilon as relative error
                    {
                      if ( reportErrors )
                      {
                        fluxfilediff << "Diff at line " << numLine << " : vcl_abs ( ("
                            << strNumRef << ") - (" << strNumTest
                            << ") ) > " << epsilon << std::endl;
                        nblinediff++;
                      }
                      nbdiff++;
                    }

                    strNumRef="";
                    strNumTest="";
                    strCharRef=charTmpRef;
                    strCharTest=charTmpTest;
                    chgt=true;
                  }
                  else if (etatCour==etatPrec)
                  {
                    if (etatCour==ETAT_CHAR)
                    {
                      strCharRef+=charTmpRef;
                      strCharTest+=charTmpTest;
                    }
                    else
                    {
                      strNumRef+=charTmpRef;
                      strNumTest+=charTmpTest;
                    }
                  }

                  etatPrec = etatCour;
                  ++i;
                }

              // Simpliest case : string characters or numeric value between 2 separators
                if (!chgt)
                {
                  if (isNumeric(strRef))
                  {

                    if ( ( strRef != strTest)
                           && (vcl_abs(atof(strRef.c_str())) > m_EpsilonBoundaryChecking)
                           && (vcl_abs(atof(strRef.c_str())-atof(strTest.c_str()))
                           > epsilon*vcl_abs(atof(strRef.c_str()))
                              )) //epsilon as relative error
                    {
                      if ( reportErrors )
                      {
                        fluxfilediff << "Diff at line " << numLine << " : vcl_abs( ("
                            << strRef << ") - (" << strTest
                            << ") ) > " << epsilon << std::endl;
                        nblinediff++;
                      }
                      nbdiff++;
                    }
                  }
                  else
                  {
                    if ( strRef != strTest )
                    {
                      if ( reportErrors )
                      {
                        fluxfilediff << "Diff at line " << numLine
                            << " : " << strRef
                            << " != " << strTest << std::endl;
                        nblinediff++;
                      }
                      nbdiff++;
                    }
                  }
                }
              } // else
            } // if(!isHexaPointerAddress(strRef))
            else
            {
              if ( reportErrors )
              {
                fluxfilediff<<"Pointer address found at line "<<numLine
                    <<" : "<<strRef
                    <<" -> comparison skipped."<<std::endl;
              }
            }
          }

          numLine++;
        //Store alls differences lines
          if ( nblinediff!=0 && reportErrors)
          {
            listStrDiffLineFileRef.push_back(strfileref);
            listStrDiffLineFileTest.push_back(strfiletest);
          }

        }
      }

    }


    while (std::getline(fluxfiletest,strfiletest) != 0)
    {
      int nblinediff(0);
      otb::StringStream buffstreamTest;
      std::string strTest = "";
      buffstreamTest << strfiletest;
      buffstreamTest >> strTest;
      fluxfilediff << "Additional line in test file: " << numLine
          << " : " << strTest
          << std::endl;
      nblinediff++;
      nbdiff++;
      if ( nblinediff!=0 && reportErrors)
      {
        listStrDiffLineFileRef.push_back(strfileref);
        listStrDiffLineFileTest.push_back(strfiletest);
      }
    }


    fluxfiletest.close();
    fluxfileref.close();
    if ( reportErrors )
    {
      fluxfilediff.close();
    }

    if ( nbdiff!=0 && reportErrors)
    {
      std::cout << "<DartMeasurement name=\"ASCIIFileError\" type=\"numeric/int\">";
      std::cout << nbdiff;
      std::cout <<  "</DartMeasurement>" << std::endl;
      std::cout << "================================================================"<<std::endl;
      std::cout << "baseline ASCII File : "<<baselineAsciiFileName << std::endl;
      std::cout << "Test ASCII File     : "<<testAsciiFileName << std::endl;
      std::cout << "Diff ASCII File     : "<<diffAsciiFileName << std::endl;
      std::cout << "Tolerance value     : "<<epsilon << std::endl;
      std::cout << "Tolerance max check : "<<m_EpsilonBoundaryChecking << std::endl;

      std::cout << "Nb lines differents : "<<listStrDiffLineFileRef.size() << std::endl;
      for ( unsigned int i = 0; i  < listStrDiffLineFileRef.size(); ++i)
      {
        std::cout << "   -------------------------------"<<std::endl;
        std::cout << "   Base << "<<listStrDiffLineFileRef[i]<<std::endl;
        std::cout << "   Test >> "<<listStrDiffLineFileTest[i]<<std::endl;
      }
    }
    return (nbdiff != 0) ? 1 : 0;
  }

/******************************************/
/******************************************/
/******************************************/
int TestHelper::RegressionTestListFile(const char * testListFileName, const char * baselineListFileName, int reportErrors, const double epsilon, std::vector<std::string> ignoredLines) const
{
  std::cout<<"RegressionTestListFileRegressionTestListFileRegressionTestListFile"<<std::endl;
  std::ifstream fluxfileref(baselineListFileName);
  std::vector<unsigned int> usedLineInTestFile;
  
  enum TypeEtat { ETAT_NUM, ETAT_CHAR };
  
  std::string diffListFileName(testListFileName);
  diffListFileName += ".diff.txt";
  std::ofstream fluxfilediff;
  if ( reportErrors )
    {
      fluxfilediff.open(diffListFileName.c_str());
    }
  
  //std::string strfiletest;
  std::string strfileref;
  
  int nbdiff(0);
  int numLine(1);

  if (!fluxfileref)
    {
      itkGenericExceptionMacro(<< "Impossible to open the baseline List file <"<<baselineListFileName<<">.");
    }
  
  TypeEtat etatPrec(ETAT_NUM), etatCour(ETAT_NUM);
  
  std::vector<std::string> listStrDiffLineFileRef;
  std::vector<std::string> listStrDiffLineFileTest;
  
  
  
  while ( std::getline(fluxfileref,strfileref)!=0  )
    {
      std::cout<<"**************** REFENC LINE: "<<strfileref<<std::endl;
      //otb::StringStream buffstreamRef;
      //buffstreamRef << strfileref;
      
      //check if we've reached end of test file
      //if (std::getline(fluxfiletest,strfiletest) == 0)
      bool foundexpr = false;
      if (ignoredLines.size()>0)
        {
	  
          std::vector<std::string>::iterator itIgnoredLines = ignoredLines.begin();
	  
          for (;(itIgnoredLines != ignoredLines.end()); ++itIgnoredLines)
	    {
	      std::string ignoredLinesList = (*itIgnoredLines);
	      std::string::size_type loc = strfileref.find(ignoredLinesList);
	      if ( loc != std::string::npos )
		{
		  foundexpr = true;
		}
	      
	    }
	  
        }      
      if ( foundexpr == false )
        {
	  bool isFound = false;
	  unsigned int lineId = 0;
	  std::string strfiletest;
	  // Open tested file
	  std::ifstream fluxfiletest(testListFileName);
	  if (!fluxfiletest)
	    {
	      itkGenericExceptionMacro(<<"Impossible to open the test List file <"<<testListFileName<<">.");
	    }

	  while ( std::getline(fluxfiletest,strfiletest)!=0 && isFound==false )
	    {
	      std::cout<<"**************** TESTED LINE: "<<strfiletest<<std::endl;
	      lineId++;
	      otb::StringStream buffstreamTest;
	      otb::StringStream buffstreamRef;
	      isFound=true;
	      buffstreamTest << strfiletest;
	      buffstreamRef << strfileref;
	      int nblinediff(0);
	      
	      while (buffstreamRef.peek() != EOF)
		{
		  std::string strRef = "";
		  std::string strTest = "";
		  
		  std::string strNumRef = "";
		  std::string strCharRef = "";
		  std::string strNumTest = "";
		  std::string strCharTest = "";
		  
		  buffstreamRef >> strRef;
		  buffstreamTest >> strTest;
		  
		  bool chgt= false;
		  std::string charTmpRef = "";
		  std::string charTmpTest = "";
		  unsigned int i=0;
		  
		  std::cout<<strRef<<std::endl;
		  if (!isHexaPointerAddress(strRef))
		    {
		      //Analyse if strRef contains scientific value (ex: "-142.124e-012")
		      if (isScientificNumeric(strRef))
			{
			  if (!isScientificNumeric(strTest))
			    {
			      isFound = false;
			    }
			  else if ( (strRef != strTest)
				    && (vcl_abs(atof(strRef.c_str())) > m_EpsilonBoundaryChecking)
				    && (vcl_abs(atof(strRef.c_str())-atof(strTest.c_str()))
					> epsilon*vcl_abs(atof(strRef.c_str()))
					) )//epsilon as relative error
			    {
			      isFound = false;
			    }
			}
		      else
			{
			  std::cout<<"!isScientificNumeric "<<epsilon<<std::endl;
			  while (i < strRef.size())
			    {
			      charTmpRef=strRef[i];
			      
			      if (i<strTest.size())
				{
				  charTmpTest=strTest[i];
				}
			      
			      if (isNumeric(charTmpRef))
				etatCour = ETAT_NUM;
			      else
				etatCour = ETAT_CHAR;
			      
			      // "reference" state initialisation.
			      if (i==0)
				etatPrec=etatCour;
			      
			      // Case where there's a number after characteres.
			      if ((etatCour==ETAT_NUM)&&(etatPrec==ETAT_CHAR))
				{
				  if ( strCharRef != strCharTest )
				    {
				      isFound = false;
				    }
				  
				  strCharRef="";
				  strCharTest="";
				  strNumRef=charTmpRef;
				  strNumTest=charTmpTest;
				  chgt=true;
				}
			      // Case where there's a character after numbers.
			      else if ((etatCour==ETAT_CHAR)&&(etatPrec==ETAT_NUM))
				{
				  if ( (strNumRef != strNumTest)
				       && (vcl_abs(atof(strNumRef.c_str())) > m_EpsilonBoundaryChecking)
				       && (vcl_abs(atof(strNumRef.c_str())-atof(strNumTest.c_str()))
					   > epsilon*vcl_abs(atof(strNumRef.c_str()))
					   ) ) //epsilon as relative error
				    {
				      isFound = false;
				    }
				  
				  strNumRef="";
				  strNumTest="";
				  strCharRef=charTmpRef;
				  strCharTest=charTmpTest;
				  chgt=true;
				}
			      else if (etatCour==etatPrec)
				{
				  if (etatCour==ETAT_CHAR)
				    {
				      strCharRef+=charTmpRef;
				      strCharTest+=charTmpTest;
				    }
				  else
				    {
				      strNumRef+=charTmpRef;
				      strNumTest+=charTmpTest;
				    }
				}
			      
			      etatPrec = etatCour;
			      ++i;
			    }
			  // Simpliest case : string characters or numeric value between 2 separators
			  if (!chgt)
			    {
			      if (isNumeric(strRef))
				{
				  if ( ( strRef != strTest)
				       && (vcl_abs(atof(strRef.c_str())) > m_EpsilonBoundaryChecking)
				       && (vcl_abs(atof(strRef.c_str())-atof(strTest.c_str()))
					   > epsilon*vcl_abs(atof(strRef.c_str()))
					   )) //epsilon as relative error
				    {
				      isFound = false;
				    }
				}
			      else
				{
				  if ( strRef != strTest )
				    {
				      isFound = false;
				    }
				}
			    }
			} // else
		    } // if(!isHexaPointerAddress(strRef))
		} // end 
	      // copy the line

	      if(isFound == true)
		{
		  bool lineAlreadyUsed = false;
		  unsigned int count = 0;
		  while(count<usedLineInTestFile.size() &&  lineAlreadyUsed==false)
		    {
		      if(usedLineInTestFile[count]==lineId)
			{
			  isFound = false;
			  lineAlreadyUsed = true;
			}
		      count++;
		    }
		  if(lineAlreadyUsed==false)
		    {
		      usedLineInTestFile.push_back(lineId);
		      std::cout<<"~~~~ Line found: "<<std::endl;
		      std::cout<<"~~~~ reference: "<<strfileref<<std::endl;
		      std::cout<<"~~~~ test     : "<<strfiletest<<std::endl;
		    }
		}
	              

	    }// end while( std::getline(fluxfiletestremoved,strfiletest)!=0 && foundexpr == false )

	  if(isFound == false)
	    {
	      std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++ Line not found: "<<strfileref<<std::endl;
	      listStrDiffLineFileRef.push_back(strfileref);
	      nbdiff++;
	    }
	  fluxfiletest.close();
	} // endif ( foundexpr == false )  
    }// end while( std::getline(fluxfileref,strfileref)!=0 )
  
  
      //fluxfiletest.close();
  fluxfileref.close();
  if ( reportErrors )
    {
      fluxfilediff.close();
    }
  

  std::ifstream fluxfiletest(testListFileName);
  std::string strfiletest;
  unsigned int testNbLines = 0;
  while ( std::getline(fluxfiletest,strfiletest) )
    {
      testNbLines++;
    }

  if( testNbLines > usedLineInTestFile.size() )
    {
      std::ifstream fluxfiletestTmp(testListFileName);
      unsigned int count = 0;
      while ( std::getline(fluxfiletestTmp,strfiletest)!=0 )
	{
	  count++;
	  bool found = false;
	  unsigned int j=0;
	  while(j<usedLineInTestFile.size() &&  found==false)
	    {
	      if(usedLineInTestFile[j]==count)
		{
		  found = true;
		}
	      j++;
	    }
	  if(found==false)
	    listStrDiffLineFileTest.push_back(strfiletest);
	  //count++;
	}
    }


  std::cout<<testNbLines<<std::endl;

  if ( nbdiff!=0 && reportErrors)
    {
      std::cout << "<DartMeasurement name=\"ListFileError\" type=\"numeric/int\">";
      std::cout << nbdiff;
      std::cout <<  "</DartMeasurement>" << std::endl;
      std::cout << "================================================================"<<std::endl;
      std::cout << "baseline List File : "<<baselineListFileName << std::endl;
      std::cout << "Test List File     : "<<testListFileName << std::endl;
      std::cout << "Diff List File     : "<<diffListFileName << std::endl;
      std::cout << "Tolerance value     : "<<epsilon << std::endl;
      std::cout << "Tolerance max check : "<<m_EpsilonBoundaryChecking << std::endl;
      
      std::cout << "Nb lines differents : "<<listStrDiffLineFileRef.size() << std::endl;
      std::cout << "Line(s) in Baseline file but not in Test file : "<<listStrDiffLineFileRef.size() << std::endl;
      for ( unsigned int i = 0; i  < listStrDiffLineFileRef.size(); ++i)
	{
	  std::cout <<listStrDiffLineFileRef[i]<<std::endl;
	}
      std::cout << "   -------------------------------"<<std::endl;
      std::cout << "Line(s) in Test file but not in Baseline file : "<<listStrDiffLineFileTest.size() << std::endl;
      for ( unsigned int i = 0; i  < listStrDiffLineFileTest.size(); ++i)
	{
	  std::cout <<listStrDiffLineFileTest[i]<<std::endl;
	}
    }
  
  return (nbdiff != 0) ? 1 : 0;
}
  
/******************************************/
/******************************************/
/******************************************/


  int TestHelper::RegressionTestBinaryFile(const char * testBinaryFileName, const char * baselineBinaryFileName, int reportErrors) const
  {
    int nbdiff(0);
    std::ifstream fluxfiletest(testBinaryFileName,std::ifstream::binary);
    std::ifstream fluxfileref(baselineBinaryFileName,std::ifstream::binary);
    if (fluxfiletest.fail())
    {
      itkGenericExceptionMacro(<<"Impossible to open the test binary file <" << testBinaryFileName << ">.");
    }
    if (!fluxfileref)
    {
      itkGenericExceptionMacro(<< "Impossible to open the baseline binary file <" << baselineBinaryFileName << ">.");
    }
    while ( fluxfiletest.good() && fluxfileref.good() )
    {
      if ( fluxfiletest.get() != fluxfileref.get() )
      {
        nbdiff++;
      }
    }
    fluxfiletest.close();
    fluxfileref.close();

    if ( nbdiff!=0 && reportErrors)
    {
      std::cout << "<DartMeasurement name=\"BINARYFileError\" type=\"numeric/int\">";
      std::cout << nbdiff;
      std::cout <<  "</DartMeasurement>" << std::endl;
    }
    return (nbdiff != 0) ? 1 : 0;
  }


  int TestHelper::RegressionTestImage (int cpt, const char *testImageFilename, const char *baselineImageFilename, int reportErrors, const double toleranceDiffPixelImage) const
  {
  // Use the factory mechanism to read the test and baseline files and convert them to double

    typedef otb::VectorImage<double,2> ImageType;
    typedef otb::VectorImage<unsigned char,2> OutputType;
    typedef otb::VectorImage<unsigned char,2> DiffOutputType;
    typedef otb::ImageFileReader<ImageType> ReaderType;

  // Read the baseline file
    ReaderType::Pointer baselineReader = ReaderType::New();
    baselineReader->SetFileName(baselineImageFilename);
    try
    {
      baselineReader->UpdateLargestPossibleRegion();
    }
    catch (itk::ExceptionObject& e)
    {
      itkGenericExceptionMacro(<< "Exception detected while reading "
          << baselineImageFilename << " : "  << e.GetDescription());
      return 1000;
    }

  // Read the file generated by the test
    ReaderType::Pointer testReader = ReaderType::New();
    testReader->SetFileName(testImageFilename);
    try
    {
      testReader->UpdateLargestPossibleRegion();
    }
    catch (itk::ExceptionObject& e)
    {
      itkGenericExceptionMacro(<< "Exception detected while reading "
          << testImageFilename << " : "  << e.GetDescription() );
      return 1000;
    }

  // The sizes of the baseline and test image must match
    ImageType::SizeType baselineSize;
    baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
    ImageType::SizeType testSize;
    testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

    if (baselineSize != testSize)
    {
      std::cerr << "The size of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has size " << baselineSize << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has size " << testSize << std::endl;
      return 1;
    }


  // Now compare the two images
    typedef otb::DifferenceImageFilter<ImageType,ImageType> DiffType;
    DiffType::Pointer diff = DiffType::New();
    diff->SetValidInput(baselineReader->GetOutput());
    diff->SetTestInput(testReader->GetOutput());
    diff->SetDifferenceThreshold(toleranceDiffPixelImage);
    diff->UpdateLargestPossibleRegion();

    ImageType::PixelType status = diff->GetTotalDifference();
    unsigned long numberOfPixelsWithDifferences = diff->GetNumberOfPixelsWithDifferences();

  //Write only one this message
    if (reportErrors == 0)
    {
      otbGenericMsgDebugMacro(<< "RegressionTestImage DifferenceThreshold: "<<toleranceDiffPixelImage);
      otbGenericMsgDebugMacro(<< "Status diff->GetTotalDifference:         "
          << status <<" for "<<numberOfPixelsWithDifferences<<" pixel(s)." );
    }
  // if there are discrepencies, create an diff image
    if (status.GetSquaredNorm()>0 && reportErrors)
    {
      typedef otb::PrintableImageFilter<ImageType> RescaleType;
      typedef RescaleType::OutputImageType OutputType;
      /*     typedef itk::ExtractImageFilter<OutputType,DiffOutputType> ExtractType; */
      typedef otb::ImageFileWriter<RescaleType::OutputImageType> WriterType;

      RescaleType::Pointer rescale = RescaleType::New();

      WriterType::Pointer writer = WriterType::New();
      writer->SetInput(rescale->GetOutput());

//    std::cout << "<DartMeasurement name=\"ImageError\" type=\"numeric/double\">";
      std::cout << "<DartMeasurement name=\"ImageError "<<cpt<<"\" type=\"numeric/double\">";
      std::cout << status;
      std::cout <<  "</DartMeasurement>" << std::endl;

      std::cout << "<DartMeasurement name=\"NumberOfPixelsWithDifferences "<<cpt<<"\" type=\"numeric/integer\">";
      std::cout << numberOfPixelsWithDifferences;
      std::cout <<  "</DartMeasurement>" << std::endl;

      std::cout << "<DartMeasurement name=\"ToleranceDiffPixelImage\" type=\"numeric/double\">";
      std::cout << toleranceDiffPixelImage;
      std::cout <<  "</DartMeasurement>" << std::endl;

        ::itk::OStringStream diffName;
        diffName << testImageFilename << ".diff.png";
        try
        {
          rescale->SetInput(diff->GetOutput());

          for (unsigned int i = 1;i<=min(diff->GetOutput()->GetNumberOfComponentsPerPixel(),3U);++i)
          {
            rescale->SetChannel(i);
          }

          rescale->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<< "Error during rescale of " << diffName.str() );
        }
        writer->SetFileName(diffName.str().c_str());
        try
        {
          writer->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<< "Error during write of " << diffName.str() );
        }

//    std::cout << "<DartMeasurementFile name=\"DifferenceImage\" type=\"image/png\">";
        std::cout << "<DartMeasurementFile name=\"DifferenceImage "<<cpt<<"\" type=\"image/png\">";
        std::cout << diffName.str();
        std::cout << "</DartMeasurementFile>" << std::endl;

        ::itk::OStringStream baseName;
        baseName << testImageFilename << ".base.png";
        try
        {
          rescale->SetInput(baselineReader->GetOutput());
          rescale->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<<"Error during rescale of " << baseName.str() );
        }
        try
        {
          writer->SetFileName(baseName.str().c_str());
          writer->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<<"Error during write of " << baseName.str() );
        }

//    std::cout << "<DartMeasurementFile name=\"baselineImage\" type=\"image/png\">";
        std::cout << "<DartMeasurementFile name=\"baselineImage "<<cpt<<"\" type=\"image/png\">";
        std::cout << baseName.str();
        std::cout << "</DartMeasurementFile>" << std::endl;

        ::itk::OStringStream testName;
        testName << testImageFilename << ".test.png";
        try
        {
          rescale->SetInput(testReader->GetOutput());
          rescale->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<< "Error during rescale of " << testName.str());
        }
        try
        {
          writer->SetFileName(testName.str().c_str());
          writer->Update();
        }
        catch (...)
        {
          itkGenericExceptionMacro(<<"Error during write of " << testName.str() );
        }

//    std::cout << "<DartMeasurementFile name=\"TestImage\" type=\"image/png\">";
        std::cout << "<DartMeasurementFile name=\"TestImage "<<cpt<<"\" type=\"image/png\">";
        std::cout << testName.str();
        std::cout << "</DartMeasurementFile>" << std::endl;


    }
    return (status.GetSquaredNorm()> 0) ? 1 : 0;
  }

  int TestHelper::RegressionTestMetaData (const char *testImageFilename, const char *baselineImageFilename, int reportErrors, const double toleranceDiffPixelImage) const
  {
  // Use the factory mechanism to read the test and baseline files and convert them to double
    typedef otb::Image<double,ITK_TEST_DIMENSION_MAX> ImageType;
    typedef otb::ImageFileReader<ImageType> ReaderType;

  // Read the baseline file
    ReaderType::Pointer baselineReader = ReaderType::New();
    baselineReader->SetFileName(baselineImageFilename);
    try
    {
      baselineReader->GenerateOutputInformation();
    }
    catch (itk::ExceptionObject& e)
    {
      itkGenericExceptionMacro(<< "Exception detected while reading "
          << baselineImageFilename << " : "  << e.GetDescription());
    }

  // Read the baseline file
    ReaderType::Pointer testReader = ReaderType::New();
    testReader->SetFileName(testImageFilename);
    try
    {
      testReader->GenerateOutputInformation();
    }
    catch (itk::ExceptionObject& e)
    {
      itkGenericExceptionMacro(<< "Exception detected while reading "
          << baselineImageFilename << " : "  << e.GetDescription());
    }

    unsigned int errcount = 0;
  // The sizes of the baseline and test image must match
    ImageType::SizeType baselineSize;
    baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
    ImageType::SizeType testSize;
    testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

    if (baselineSize != testSize)
    {
      std::cerr << "The size of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has size " << baselineSize << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has size " << testSize << std::endl;
      errcount++;
    }
    ImageType::Pointer blImPtr = baselineReader->GetOutput();
    ImageType::Pointer testImPtr = testReader->GetOutput();

  // test orgin
    if (blImPtr->GetOrigin()!=testImPtr->GetOrigin())
    {
      std::cerr << "The origin of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has origin " << blImPtr->GetOrigin() << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has origin "<< testImPtr->GetOrigin() << std::endl;
      errcount++;
    }

  // test spacing
    if (blImPtr->GetSpacing()!=testImPtr->GetSpacing())
    {
      std::cerr << "The spacing of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has spacing " << blImPtr->GetSpacing() << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has spacing "<< testImPtr->GetSpacing() << std::endl;
      errcount++;
    }

  // test projection reference
    if (blImPtr->GetProjectionRef().compare(testImPtr->GetProjectionRef())!=0)
    {
      std::cerr << "The projection reference of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has projection reference " << blImPtr->GetProjectionRef() << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has projection reference " << testImPtr->GetProjectionRef() << std::endl;
      errcount++;
    }

  // test Geographic transform
    if (blImPtr->GetGeoTransform()!=testImPtr->GetGeoTransform())
    {
      std::cerr << "The geographic transform of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has geographic transform " << VectorToString(blImPtr->GetGeoTransform()) << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has geographic transform " << VectorToString(testImPtr->GetGeoTransform()) << std::endl;
      errcount++;
    }

// test upper left corner
    if (blImPtr->GetUpperLeftCorner()!=testImPtr->GetUpperLeftCorner())
    {
      std::cerr << "The upper left corner of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has upper left corner " << VectorToString(blImPtr->GetUpperLeftCorner()) << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has upper left corner " <<VectorToString( testImPtr->GetUpperLeftCorner()) << std::endl;
      errcount++;
    }

// test upper right corner
    if (blImPtr->GetUpperRightCorner()!=testImPtr->GetUpperRightCorner())
    {
      std::cerr << "The upper right corner of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has upper right corner " <<VectorToString( blImPtr->GetUpperRightCorner()) << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has upper right corner " << VectorToString(testImPtr->GetUpperRightCorner()) << std::endl;
      errcount++;
    }

// test lower left corner
    if (blImPtr->GetLowerLeftCorner()!=testImPtr->GetLowerLeftCorner())
    {
      std::cerr << "The lower left corner  of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has lower left corner " << VectorToString(blImPtr->GetLowerLeftCorner()) << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has lower left corner " << VectorToString(testImPtr->GetLowerLeftCorner()) << std::endl;
      errcount++;
    }

  // test lower right corner
    if (blImPtr->GetLowerRightCorner()!=testImPtr->GetLowerRightCorner())
    {
      std::cerr << "The lower right of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has lower right corner " << VectorToString(blImPtr->GetLowerRightCorner()) << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has lower right corner " << VectorToString(testImPtr->GetLowerRightCorner()) << std::endl;
      errcount++;
    }

  // test gcp projection
    if (blImPtr->GetGCPProjection().compare(testImPtr->GetGCPProjection())!=0)
    {
      std::cerr << "The gcp projection of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has gcp projection " << blImPtr->GetGCPProjection() << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has gcp projection " << testImPtr->GetGCPProjection() << std::endl;
      errcount++;
    }


  // test gcp count
    if (blImPtr->GetGCPCount()!=testImPtr->GetGCPCount())
    {
      std::cerr << "The gcp count of the baseline image and Test image do not match!" << std::endl;
      std::cerr << "baseline image: " << baselineImageFilename
          << " has gcp count " << blImPtr->GetGCPCount() << std::endl;
      std::cerr << "Test image:     " << testImageFilename
          << " has gcp count " << testImPtr->GetGCPCount() << std::endl;
      errcount++;
    }
    else
    {
      for (unsigned int i=0;i<blImPtr->GetGCPCount();++i)
      {
        if ((blImPtr->GetGCPId(i).compare(testImPtr->GetGCPId(i))!=0)
             ||(blImPtr->GetGCPInfo(i).compare(testImPtr->GetGCPInfo(i))!=0)
             ||(blImPtr->GetGCPRow(i)!=testImPtr->GetGCPRow(i))
             ||(blImPtr->GetGCPCol(i)!=testImPtr->GetGCPCol(i))
             ||(blImPtr->GetGCPX(i)!=testImPtr->GetGCPX(i))
             ||(blImPtr->GetGCPY(i)!=testImPtr->GetGCPY(i))
             ||(blImPtr->GetGCPZ(i)!=testImPtr->GetGCPZ(i)))
        {
          std::cerr << "The GCP number "<<i<<" of the baseline image and Test image do not match!" << std::endl;
          std::cerr << "baseline image: " << baselineImageFilename
              << " has gcp number "<<i<<" ("
              <<"id: "<<blImPtr->GetGCPId(i)<<", "
              <<"info: "<<blImPtr->GetGCPInfo(i)<<", "
              <<"row: "<<blImPtr->GetGCPRow(i)<<", "
              <<"col: "<<blImPtr->GetGCPCol(i)<<", "
              <<"X: "<<blImPtr->GetGCPX(i)<<", "
              <<"Y: "<<blImPtr->GetGCPY(i)<<", "
              <<"Z: "<<blImPtr->GetGCPZ(i)<<")"<<std::endl;
          std::cerr << "Test image:     " << testImageFilename
              << " has gcp  number "<<i<<" ("
              <<"id: "<<testImPtr->GetGCPId(i)<<", "
              <<"info: "<<testImPtr->GetGCPInfo(i)<<", "
              <<"row: "<<testImPtr->GetGCPRow(i)<<", "
              <<"col: "<<testImPtr->GetGCPCol(i)<<", "
              <<"X: "<<testImPtr->GetGCPX(i)<<", "
              <<"Y: "<<testImPtr->GetGCPY(i)<<", "
              <<"Z: "<<testImPtr->GetGCPZ(i)<<")"<<std::endl;
          errcount++;
        }
      }
    }
    if (errcount>0)
    {
      std::cout << "<DartMeasurement name=\"MetadataError\" type=\"numeric/int\">";
      std::cout << errcount;
      std::cout <<  "</DartMeasurement>" << std::endl;
    }
    return errcount;

  }


  //
// Generate all of the possible baselines
// The possible baselines are generated fromn the baselineFilename using the following algorithm:
// 1) strip the suffix
// 2) append a digit _x
// 3) append the original suffix.
// It the file exists, increment x and continue
  //
  std::map<std::string,int>
      TestHelper::RegressionTestbaselines (char *baselineFilename) const
  {
    std::map<std::string,int> baselines;
    baselines[std::string(baselineFilename)] = 0;

    std::string originalBaseline(baselineFilename);

    int x = 0;
    std::string::size_type suffixPos = originalBaseline.rfind(".");
    std::string suffix;
    if (suffixPos != std::string::npos)
    {
      suffix = originalBaseline.substr(suffixPos,originalBaseline.length());
      originalBaseline.erase(suffixPos,originalBaseline.length());
    }
    while (++x)
    {
        ::itk::OStringStream filename;
        filename << originalBaseline << "." << x << suffix;
        std::ifstream filestream(filename.str().c_str());
        if (!filestream)
        {
          break;
        }
        baselines[filename.str()] = 0;
        filestream.close();
    }
    return baselines;
  }


  int TestHelper::RegressionTestOgrFile (const char *testOgrFilename, const char *baselineOgrFilename, int reportErrors, const double toleranceDiffValue) const
  {
    const char  *ref_pszDataSource = baselineOgrFilename;
    const char  *test_pszDataSource = testOgrFilename;
    const char *ref_pszWHERE = NULL;
    const char *test_pszWHERE = NULL;
    int     bReadOnly = FALSE;
    int     bVerbose = reportErrors;
    int     nbdiff(0);
/* -------------------------------------------------------------------- */
/*      Open data source.                                               */
/* -------------------------------------------------------------------- */
    OGRDataSource       *ref_poDS = NULL;
    OGRSFDriver         *ref_poDriver = NULL;
    OGRGeometry         *ref_poSpatialFilter = NULL;
    OGRDataSource       *test_poDS = NULL;
    OGRSFDriver         *test_poDriver = NULL;
    OGRGeometry         *test_poSpatialFilter = NULL;

    OGRRegisterAll();

    ref_poDS = OGRSFDriverRegistrar::Open( ref_pszDataSource, !bReadOnly, &ref_poDriver );
    if( ref_poDS == NULL && !bReadOnly )
    {
        ref_poDS = OGRSFDriverRegistrar::Open( ref_pszDataSource, FALSE, &ref_poDriver );
        if( ref_poDS != NULL && bVerbose )
        {
            std::cout << "Had to open REF data source read-only.\n";
            bReadOnly = TRUE;
        }
    }
    test_poDS = OGRSFDriverRegistrar::Open( test_pszDataSource, !bReadOnly, &test_poDriver );
    if( test_poDS == NULL && !bReadOnly )
    {
        test_poDS = OGRSFDriverRegistrar::Open( test_pszDataSource, FALSE, &test_poDriver );
        if( test_poDS != NULL && bVerbose )
        {
            std::cout << "Had to open REF data source read-only.\n";
            bReadOnly = TRUE;
        }
    }
/* -------------------------------------------------------------------- */
/*      Report failure                                                  */
/* -------------------------------------------------------------------- */
    if( ref_poDS == NULL )
    {
        OGRSFDriverRegistrar    *ref_poR = OGRSFDriverRegistrar::GetRegistrar();
        
        if(bVerbose) std::cout << "FAILURE:\n"
                "Unable to open REF datasource `"<<ref_pszDataSource<<"' with the following drivers."<<std::endl;
        for( int iDriver = 0; iDriver < ref_poR->GetDriverCount(); ++iDriver )
        {
            std::cout<< "  -> "<< ref_poR->GetDriver(iDriver)->GetName() <<std::endl;
        }
        return( 1 );
    }
    CPLAssert( ref_poDriver != NULL);

    if( test_poDS == NULL )
    {
        OGRSFDriverRegistrar    *test_poR = OGRSFDriverRegistrar::GetRegistrar();
        
        if(bVerbose) std::cout << "FAILURE:\n"
                "Unable to open TEST datasource `"<<test_pszDataSource<<"' with the following drivers."<<std::endl;
        for( int iDriver = 0; iDriver < test_poR->GetDriverCount(); ++iDriver )
        {
            std::cout<< "  -> "<< test_poR->GetDriver(iDriver)->GetName() <<std::endl;
        }
        return( 1 );
    }
    CPLAssert( test_poDriver != NULL);

/* -------------------------------------------------------------------- */
/*      Some information messages.                                      */
/* -------------------------------------------------------------------- */
    otbCheckStringValue("INFO: using driver", ref_poDriver->GetName(),test_poDriver->GetName(),nbdiff,bVerbose );


//    otbCheckStringValue("INFO: Internal data source name", ref_poDS->GetName(),test_poDS->GetName(),nbdiff,bVerbose );
    std::string strRefName(ref_poDS->GetName());
    std::string strTestName(test_poDS->GetName());
    if( strRefName != strTestName)
    {
        if(!bVerbose) otbPrintDiff("WARNING: INFO: Internal data source name poDS->GetName() were different",strRefName,strTestName);
    } 

/* -------------------------------------------------------------------- */
/*      Process each data source layer.                                 */
/* -------------------------------------------------------------------- */
    otbCheckValue("GetLayerCount()", ref_poDS->GetLayerCount(),test_poDS->GetLayerCount(),nbdiff,bVerbose );

        for( int iLayer = 0; iLayer < ref_poDS->GetLayerCount(); ++iLayer )
        {
            OGRLayer        *ref_poLayer = ref_poDS->GetLayer(iLayer);
            OGRLayer        *test_poLayer = test_poDS->GetLayer(iLayer);

            if( ref_poLayer == NULL )
            {
                if(bVerbose) std::cout<< "FAILURE: Couldn't fetch advertised layer "<<iLayer<<" for REF data source"<<std::endl;
                return(1);
            }
            if( test_poLayer == NULL )
            {
                if(bVerbose) std::cout<< "FAILURE: Couldn't fetch advertised layer "<<iLayer<<" for REF data source"<<std::endl;
                return(1);
            }

            //Check Layer inforamtion
            ogrReportOnLayer( ref_poLayer, ref_pszWHERE, ref_poSpatialFilter, test_poLayer, test_pszWHERE, test_poSpatialFilter, nbdiff, bVerbose);

            //If no difference, check the feature 
            if(nbdiff == 0)
            {
                OGRFeature  *ref_poFeature = NULL;
                OGRFeature  *test_poFeature = NULL;
                std::string basefilename(test_pszDataSource);
                int nbFeature(0);
                otb::StringStream oss2;
                oss2.str("");
                oss2 << iLayer;

                while( (ref_poFeature = ref_poLayer->GetNextFeature()) != NULL )
                {
                    test_poFeature = test_poLayer->GetNextFeature();
                    otb::StringStream oss;
                    oss.str("");
                    oss << nbFeature;

                    std::string ref_filename  = basefilename + "_temporary_layer_"+oss2.str()+"_feature_"+oss.str()+"_ref.txt";
                    std::string test_filename = basefilename + "_temporary_layer_"+oss2.str()+"_feature_"+oss.str()+"_test.txt";
                    FILE *ref_f(NULL);
                    ref_f = fopen( ref_filename.c_str(), "w" );
                    if(ref_f == NULL)
                    {
                      itkGenericExceptionMacro(<<"Impossible to create ASCII file <"<<ref_filename<<">.");
                    }
                    ref_poFeature->DumpReadable( ref_f );
                    delete ref_poFeature;
                    fclose(ref_f);

                    FILE *test_f(NULL);
                    test_f = fopen( test_filename.c_str(), "w" );
                    if(test_f == NULL)
                    {
                      itkGenericExceptionMacro(<<"Impossible to create ASCII file <"<<test_filename<<">.");
                    }
                    test_poFeature->DumpReadable( test_f );
                    delete test_poFeature;
                    fclose(test_f);

                    //Check ASCII comparaison
                    std::vector<std::string> ignoredLines;
                    ignoredLines.clear();
                    nbdiff = RegressionTestAsciiFile(test_filename.c_str(),ref_filename.c_str(),bVerbose,toleranceDiffValue,ignoredLines);

                    nbFeature++;
                }
                // If no verbose and an diff was found, exit checking. The full checking will be executed in verbose mode 
                if( (!bVerbose) && (nbdiff!=0) ) return (1);
            } //if(nbdiff == 0)

        }

/* -------------------------------------------------------------------- */
/*      Close down.                                                     */
/* -------------------------------------------------------------------- */
/*    CSLDestroy( papszArgv );
    CSLDestroy( papszLayers );
    CSLDestroy( papszOptions );*/
    delete ref_poDS;
    if (ref_poSpatialFilter)
        delete ref_poSpatialFilter;
    delete test_poDS;
    if (test_poSpatialFilter)
        delete test_poSpatialFilter;

    delete OGRSFDriverRegistrar::GetRegistrar();

    OSRCleanup();
    CPLFinderClean();
    VSICleanupFileManager();
    CPLFreeConfig();
    CPLCleanupTLS();

    return (nbdiff != 0) ? 1 : 0;

  }


// Regression Testing Code

  bool TestHelper::isNumber(int i) const
  {
    return ((i>47)&&(i<58));
  }

  bool TestHelper::isHexaNumber(int i) const
  {
    return (((i>47)&&(i<58))||((i>96)&&(i<103)));
  }


  bool TestHelper::isPoint(int i) const
  {
    return (i==46);
  }

  bool TestHelper::isMinusSign(int i) const
  {
    return (i==45);
  }

  bool TestHelper::isNumeric(std::string str) const
  {
    int nbOfPoints = 0;
    int nbOfNumbers = 0;
    int number;
    unsigned int i = 0;
    bool result = true;

    while ((i<str.size())&&(result==true))
    {
      number = str[i];

      if (isPoint(number))
        ++nbOfPoints;
      if (isNumber(number))
        ++nbOfNumbers;
      if ((!isNumber(number)&&!isPoint(number)&&!isMinusSign(number))
            ||(isMinusSign(number)&&(i!=0)))
        result = false;

      ++i;
    }
    if ((str.size()==0)||(nbOfPoints > 1)/*||(nbOfNumbers==0)*/)
      result = false;

    return result;
  }

  bool TestHelper::isScientificNumeric(std::string str) const
  {

    int number(0);
    number = str[0];
    bool pointDetected(false);
    bool eDetected(false);
    bool signDetected(false);

  // Analyse first character (+, -, 0...9)
    unsigned int cpt(0);
    if ( (str[0] != '+') && (str[0] != '-') && (!isNumber(number)) ) return false;
    if ( (str[0] == '+') || (str[0] == '-') )
    {
      cpt++;
      signDetected = true;
    }

    while ( cpt < str.size() )
    {
      if ( str[cpt] == '.' )
      {
      // Exit false if two points or if e already detected
        if ( ( pointDetected == true ) || ( eDetected == true ) ) return false;
        pointDetected = true;
      }
      else if ( ( str[cpt] == 'e' )||( str[cpt] == 'E' ) )
      {
      // Exit false if two e
        if ( eDetected == true ) return false;
        eDetected = true;
      }
      else if ( ( str[cpt] == '-' )||( str[cpt] == '+' ) )
      {
      // Exit false if already sign with no E detected
        if ( ( signDetected == true ) && ( eDetected == false ) )return false;
        signDetected = true;
      }
      else
      {
        number = str[cpt];
        if (!isNumber(number)) return false;
      }
      cpt++;
    }

    return true;
  }

  bool TestHelper::isHexaPointerAddress(std::string str) const
  {
    unsigned int size(0);
    bool result(false);
    unsigned int start(0);
  //If (0xadresss)
    if (  (str[0]==40) &&
           (str[str.size()-1]==41) &&
           (str[1]==48) &&
           (str[2]==120) )
    {
      result = true;
      start = 3;
      size = str.size() - 1;
    }
  //If 0xadresss
    else if (
             (str[0]==48) &&
             (str[1]==120) )
    {
      result = true;
      start = 2;
      size = str.size();
    }
  // if 01adress
    else if (
             (str[0]==48) &&
             (str[1]==49) )
    {
      result = true;
      start = 2;
      size = str.size();
    }

    unsigned int i(start);
    while (result&&(i<size))
    {
      result=result&&isHexaNumber(str[i]);
      ++i;
    }
    return result;
  }

  std::string TestHelper::VectorToString(otb::MetaDataKey::VectorType vector) const
  {
    otb::StringStream oss;
    oss.str("");
    otb::MetaDataKey::VectorType::iterator it = vector.begin();
    oss<<"[";
    while (it!=vector.end())
    {
      oss<<(*it);
      ++it;
      if (it==vector.end())
      {
        oss<<"]";
        break;
      }
      else
      {
        oss<<", ";
      }
    }
    return oss.str();
  }


/************************************************************************/
/*                           ReportOnLayer()                            */
/************************************************************************/

void TestHelper::ogrReportOnLayer( 
    OGRLayer * ref_poLayer,   const char *ref_pszWHERE,   OGRGeometry *ref_poSpatialFilter,
    OGRLayer * test_poLayer,  const char *test_pszWHERE,  OGRGeometry *test_poSpatialFilter,
                           int & nbdiff, int bVerbose) const

{
    OGRFeatureDefn      *ref_poDefn = ref_poLayer->GetLayerDefn();
    OGRFeatureDefn      *test_poDefn = test_poLayer->GetLayerDefn();

/* -------------------------------------------------------------------- */
/*      Set filters if provided.                                        */
/* -------------------------------------------------------------------- */
    otbCheckStringValue("pszWHERE",ref_pszWHERE,test_pszWHERE,nbdiff,bVerbose);

    if( ref_pszWHERE != NULL )
        ref_poLayer->SetAttributeFilter( ref_pszWHERE );

    if( ref_poSpatialFilter != NULL )
        ref_poLayer->SetSpatialFilter( ref_poSpatialFilter );

    if( test_pszWHERE != NULL )
        test_poLayer->SetAttributeFilter( test_pszWHERE );

    if( test_poSpatialFilter != NULL )
        test_poLayer->SetSpatialFilter( test_poSpatialFilter );

/* -------------------------------------------------------------------- */
/*      Report various overall information.                             */
/* -------------------------------------------------------------------- */
    printf( "\n" );
    
    otbCheckStringValue("Layer name", ref_poDefn->GetName() ,test_poDefn->GetName() ,nbdiff,bVerbose);

    otbCheckStringValue( "Geometry", OGRGeometryTypeToName( ref_poDefn->GetGeomType() ),  OGRGeometryTypeToName( test_poDefn->GetGeomType() ) ,nbdiff,bVerbose );
        
    otbCheckValue("Feature Count", ref_poLayer->GetFeatureCount(),test_poLayer->GetFeatureCount() ,nbdiff,bVerbose);
        
        OGREnvelope ref_oExt;
        OGREnvelope test_oExt;

    otbCheckValue("GetExtent",ref_poLayer->GetExtent(&ref_oExt, TRUE),test_poLayer->GetExtent(&test_oExt, TRUE),nbdiff,bVerbose);

        if (ref_poLayer->GetExtent(&ref_oExt, TRUE) == OGRERR_NONE)
        {
            otbCheckValue("Extent: MinX",ref_oExt.MinX,test_oExt.MinX,nbdiff,bVerbose);
            otbCheckValue("Extent: MinY",ref_oExt.MinY,test_oExt.MinY,nbdiff,bVerbose);
            otbCheckValue("Extent: MaxX",ref_oExt.MaxX,test_oExt.MaxX,nbdiff,bVerbose);
            otbCheckValue("Extent: MaxY",ref_oExt.MaxY,test_oExt.MaxY,nbdiff,bVerbose);
        }

        char    *ref_pszWKT;
        char    *test_pszWKT;
        
        if( ref_poLayer->GetSpatialRef() == NULL )
            ref_pszWKT = CPLStrdup( "(unknown)" );
        else
        {
            ref_poLayer->GetSpatialRef()->exportToPrettyWkt( &ref_pszWKT );
        }            
        if( test_poLayer->GetSpatialRef() == NULL )
            test_pszWKT = CPLStrdup( "(unknown)" );
        else
        {
            test_poLayer->GetSpatialRef()->exportToPrettyWkt( &test_pszWKT );
        }            

    otbCheckStringValue( "Layer SRS WKT", ref_pszWKT,test_pszWKT ,nbdiff,bVerbose);

        CPLFree( ref_pszWKT );
        CPLFree( test_pszWKT );
    
    otbCheckStringValue( "FID Column", ref_poLayer->GetFIDColumn(),test_poLayer->GetFIDColumn() ,nbdiff,bVerbose);
    otbCheckStringValue( "Geometry Column", ref_poLayer->GetGeometryColumn(),test_poLayer->GetGeometryColumn() ,nbdiff,bVerbose);
    otbCheckValue("GetFieldCount",ref_poDefn->GetFieldCount(),test_poDefn->GetFieldCount(),nbdiff,bVerbose);
    if( ref_poDefn->GetFieldCount() == test_poDefn->GetFieldCount())
    {
        for( int iAttr = 0; iAttr < ref_poDefn->GetFieldCount(); ++iAttr )
        {
            OGRFieldDefn    *ref_poField = ref_poDefn->GetFieldDefn( iAttr );
            OGRFieldDefn    *test_poField = test_poDefn->GetFieldDefn( iAttr );
            
            otbCheckStringValue( "Field GetName",ref_poField->GetNameRef(),test_poField->GetNameRef(),nbdiff,bVerbose);
            otbCheckStringValue( "Field GetFieldTypeName",ref_poField->GetFieldTypeName( ref_poField->GetType() ),test_poField->GetFieldTypeName( test_poField->GetType() ),nbdiff,bVerbose);
            otbCheckValue( "Field GetWidth",ref_poField->GetWidth(),test_poField->GetWidth(),nbdiff,bVerbose);
            otbCheckValue( "Field GetPrecision",ref_poField->GetPrecision(),test_poField->GetPrecision(),nbdiff,bVerbose);
        }
    }

/* -------------------------------------------------------------------- */
/*      Read, and dump features.                                        */
/* -------------------------------------------------------------------- */

//Check the feature contains only if no differences found

}


}

