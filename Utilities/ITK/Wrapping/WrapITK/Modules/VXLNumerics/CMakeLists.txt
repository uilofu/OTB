BEGIN_WRAPPER_LIBRARY(VXLNumerics)

SET(WRAPPER_LIBRARY_GROUPS
  vcl_complex
  vnl_matrix
  vnl_vector
)

SET(WRAPPER_LIBRARY_LINK_LIBRARIES ITKCommon)

# don't include all the default include for that module
SET(include_save ${WRAPPER_DEFAULT_INCLUDE})
SET(WRAPPER_DEFAULT_INCLUDE)

WRAPPER_LIBRARY_CREATE_WRAP_FILES() 

# restore the default values for the other modules
SET(WRAPPER_DEFAULT_INCLUDE ${include_save})



WRAPPER_LIBRARY_CREATE_LIBRARY()

