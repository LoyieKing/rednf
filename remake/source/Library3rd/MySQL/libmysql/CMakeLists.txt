# Copyright (C) 2006 MySQL AB
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
include ( "${PROJECT_SOURCE_DIR}/win/mysql_manifest.cmake" )

# Note that we don't link with the libraries "strings" or "mysys"
# here, instead we recompile the files needed and include them
# directly. This means we don't have to worry here about if these
# libraries are compiled defining USE_TLS or not. Not that it *should*
# have been a problem anyway, they don't use thread local storage.

include_directories ( ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/libmysql ${CMAKE_SOURCE_DIR}/regex 
  ${CMAKE_SOURCE_DIR}/sql ${CMAKE_SOURCE_DIR}/strings )

# We include the source file listing instead of referencing the
# libraries. At least with CMake 2.4 and Visual Studio 2005 a static
# library created from other static libraries would not be complete,
# i.e. the libraries listed in TARGET_LINK_LIBRARIES() were just
# ignored.

# Include and add the directory path
set ( SOURCE_SUBLIBS TRUE )
set ( LIB_SOURCES "" )

if ( NOT WITH_EXTERNAL_ZLIB )
  include ( ${CMAKE_SOURCE_DIR}/extlib/zlib/CMakeLists.txt )
  foreach ( rpath ${ZLIB_SOURCES} )
  set ( LIB_SOURCES ${LIB_SOURCES} ../extlib/zlib/${rpath} )
  endforeach ( rpath )
endif ( NOT WITH_EXTERNAL_ZLIB )

include ( ${CMAKE_SOURCE_DIR}/extlib/dbug/CMakeLists.txt )
foreach ( rpath ${DBUG_SOURCES} )
set ( LIB_SOURCES ${LIB_SOURCES} ../extlib/dbug/${rpath} )
endforeach ( rpath )

if ( NOT SKIP_SSL AND NOT WITH_OPENSSL )
  include ( ${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/CMakeLists.txt )
  foreach ( rpath ${TAOCRYPT_SOURCES} )
  set ( LIB_SOURCES ${LIB_SOURCES} ../extlib/yassl/taocrypt/${rpath} )
  endforeach ( rpath )

  include ( ${CMAKE_SOURCE_DIR}/extlib/yassl/CMakeLists.txt )
  foreach ( rpath ${YASSL_SOURCES} )
  set ( LIB_SOURCES ${LIB_SOURCES} ../extlib/yassl/${rpath} )
  endforeach ( rpath )
endif ( NOT SKIP_SSL AND NOT WITH_OPENSSL )

include ( ${CMAKE_SOURCE_DIR}/mysys/CMakeLists.txt )
foreach ( rpath ${MYSYS_SOURCES} )
set ( LIB_SOURCES ${LIB_SOURCES} ../mysys/${rpath} )
endforeach ( rpath )

include ( ${CMAKE_SOURCE_DIR}/strings/CMakeLists.txt )
foreach ( rpath ${STRINGS_SOURCES} )
set ( LIB_SOURCES ${LIB_SOURCES} ../strings/${rpath} )
endforeach ( rpath )

include ( ${CMAKE_SOURCE_DIR}/vio/CMakeLists.txt )
foreach ( rpath ${VIO_SOURCES} )
set ( LIB_SOURCES ${LIB_SOURCES} ../vio/${rpath} )
endforeach ( rpath )

set ( CLIENT_SOURCES client.c errmsg.c get_password.c libmysql.c my_time.c net_serv.c 
  pack.c password.c ${LIB_SOURCES} )

# ADD_LIBRARY(mysqlclient       STATIC ${CLIENT_SOURCES})
# TARGET_LINK_LIBRARIES(mysqlclient ${CMAKE_THREAD_LIBS_INIT})
# IF(MATH_LIBRARY)
# TARGET_LINK_LIBRARIES(mysqlclient ${MATH_LIBRARY})
# ENDIF(MATH_LIBRARY)
# IF(OPENSSL_LIBRARIES)
# TARGET_LINK_LIBRARIES(mysqlclient ${OPENSSL_LIBRARIES} ${OPENSSL_LIBCRYPTO})
# ENDIF(OPENSSL_LIBRARIES)
# IF(SOCKET_LIBRARY)
# TARGET_LINK_LIBRARIES(mysqlclient ${SOCKET_LIBRARY})
# ENDIF(SOCKET_LIBRARY)
# TARGET_LINK_LIBRARIES(mysqlclient ${ZLIB_LIBRARIES})

#ADD_LIBRARY(mysql        ${CLIENT_SOURCES} libmysql.def)
add_library ( mysql ${CLIENT_SOURCES} )
if ( WIN32 AND ( CMAKE_GENERATOR MATCHES "MSYS" OR CMAKE_GENERATOR MATCHES "MinGW" 
  ) )
  target_link_libraries ( mysql ${CMAKE_THREAD_LIBS_INIT} ws2_32 )
else ( WIN32 AND ( CMAKE_GENERATOR MATCHES "MSYS" OR CMAKE_GENERATOR MATCHES "MinGW" 
  ) )
  target_link_libraries ( mysql ${CMAKE_THREAD_LIBS_INIT} )
endif ( WIN32 AND ( CMAKE_GENERATOR MATCHES "MSYS" OR CMAKE_GENERATOR MATCHES "MinGW" 
  ) )
string ( REGEX REPLACE "\\..+" "" LIBMYSQL_SOVERSION ${SHARED_LIB_VERSION} )
#SET_TARGET_PROPERTIES(mysql
#                      PROPERTIES VERSION ${SHARED_LIB_VERSION}
#                                 SOVERSION ${LIBMYSQL_SOVERSION})
if ( OPENSSL_LIBRARIES )
  target_link_libraries ( mysql ${OPENSSL_LIBRARIES} ${OPENSSL_LIBCRYPTO} )
endif ( OPENSSL_LIBRARIES )
if ( SOCKET_LIBRARY )
  target_link_libraries ( mysql ${SOCKET_LIBRARY} )
endif ( SOCKET_LIBRARY )
target_link_libraries ( mysql ${ZLIB_LIBRARIES} )
if ( MATH_LIBRARY )
  target_link_libraries ( mysql ${MATH_LIBRARY} )
endif ( MATH_LIBRARY )

# IF(NOT WIN32)
# ADD_CUSTOM_COMMAND(OUTPUT "libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}"
# "libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}"
# COMMAND ${CMAKE_COMMAND} ARGS -E remove -f libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}
# COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink libmysql${CMAKE_SHARED_LIBRARY_SUFFIX} libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}
# COMMAND ${CMAKE_COMMAND} ARGS -E remove -f libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}
# COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink libmysql${CMAKE_SHARED_LIBRARY_SUFFIX} libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}
# WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libmysql
# DEPENDS libmysql)

# ADD_CUSTOM_TARGET(LIBMYSQL_SYMLINKS
# ALL
# DEPENDS "libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}"
# "libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}")
# ENDIF(NOT WIN32)

# IF(NOT WIN32)
# # Avoid a redundant 'lib'
# SET_TARGET_PROPERTIES(libmysql
# PROPERTIES PREFIX "")
# ENDIF(NOT WIN32)

if ( EMBED_MANIFESTS )
  mysql_embed_manifest ( "myTest" "asInvoker" )
endif ( EMBED_MANIFESTS )

# Installation
#

install_library ( mysql )

