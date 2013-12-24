include_directories(${sblind_SOURCE_DIR}/sblind/include)

#find_package(LibXML2)
#include_directories(${LibXML2_INCLUDE_DIR})
#list(APPEND OPTLIBS ${LibXML2_LIBRARIES})

find_package(PkgConfig)
find_package(Threads)
pkg_check_modules(PC_LIBXML QUIET libxml-2.0)
set(LIBXML2_DEFINITIONS ${PC_LIBXML_CFLAGS_OTHER})

find_path(LIBXML2_INCLUDE_DIR libxml/xpath.h
          HINTS ${PC_LIBXML_INCLUDEDIR} ${PC_LIBXML_INCLUDE_DIRS}
          PATH_SUFFIXES libxml2 )

find_library(LIBXML2_LIBRARY NAMES xml2 libxml2
             HINTS ${PC_LIBXML_LIBDIR} ${PC_LIBXML_LIBRARY_DIRS} )

set(LIBXML2_LIBRARIES ${LIBXML2_LIBRARY} )
set(LIBXML2_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR} )  

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibXml2  DEFAULT_MSG
                                  LIBXML2_LIBRARY LIBXML2_INCLUDE_DIR
				  LIBXML2_INCLUDE_DIRS)

mark_as_advanced(LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY LIBXML2_INCLUDE_DIRS )

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  MESSAGE(STATUS "Curl libraries found at: ${CURL_LIBRARIES}")
  MESSAGE(STATUS "Curl includes found at: ${CURL_INCLUDE_DIRS}")
else()
  MESSAGE(SEND_ERROR "Could not find cURL on your system")
ENDIF(CURL_FOUND)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

include_directories(
    /usr/include/libxml2
    ${CURL_INCLUDE_DIRS}
)

add_executable(sb sb.c sb_htpparser.c sb_engines.c sb_hash.c sb_random.c sb_go.c)
target_link_libraries(sb ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES})

