# Copyright (C) 2009 Sun Microsystems, Inc.
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

include_directories ( ${CMAKE_SOURCE_DIR}/include )

# Figure out additional libraries for use with -lmysql
foreach ( dep ${libmysql_LIB_DEPENDS} )
string ( REGEX MATCH "^-.*$" out "${dep}" )
if ( out )
  set ( extra_dynamic_LDFLAGS "${extra_dynamic_LDFLAGS} ${dep}" )
endif ( out )
endforeach ( dep )

string ( REGEX REPLACE "\([^\\]\) " "\\1\\\\ " CMAKE_INSTALL_PREFIX_ESCAPED "${CMAKE_INSTALL_PREFIX}" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/mysql_config.c.in ${CMAKE_CURRENT_BINARY_DIR}/mysql_config.c 
  @ONLY )

add_executable ( mysql_config mysql_config.c )
target_link_libraries ( mysql_config mysql ${WIN32_ADD_LIBRARIES} )

# Installation
#
install ( TARGETS mysql_config DESTINATION ${INSTALL_BIN} )
