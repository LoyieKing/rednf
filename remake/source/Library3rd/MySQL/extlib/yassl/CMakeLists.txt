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

configure_file ( ${CMAKE_SOURCE_DIR}/include/my_config.h.in ${CMAKE_SOURCE_DIR}/extlib/yassl/include/config.h )
add_definitions ( "-DHAVE_CONFIG_H" )

include_directories ( ${CMAKE_SOURCE_DIR}/extlib/yassl/include ${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/include 
  ${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/mySTL ${CMAKE_SOURCE_DIR}/include )

add_definitions ( "-D_LIB -DYASSL_PREFIX" )

set ( YASSL_SOURCES src/buffer.cpp src/cert_wrapper.cpp src/crypto_wrapper.cpp src/handshake.cpp 
  src/lock.cpp src/log.cpp src/socket_wrapper.cpp src/ssl.cpp src/timer.cpp src/yassl_error.cpp 
  src/yassl_imp.cpp src/yassl_int.cpp )

set_source_files_properties ( ${YASSL_SOURCES} LANGUAGE C )

if ( NOT SOURCE_SUBLIBS )
  add_library ( yassl STATIC ${YASSL_SOURCES} )
endif ( NOT SOURCE_SUBLIBS )
