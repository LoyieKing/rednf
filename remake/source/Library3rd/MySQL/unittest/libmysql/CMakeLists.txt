# Copyright (C) 2008 Sun Microsystems, Inc.
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

enable_testing ( )

set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSAFEMALLOC -DSAFE_MUTEX" )
set ( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSAFEMALLOC -DSAFE_MUTEX" )

include_directories ( ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/unittest/mytap )

set ( API_TESTS "basic-t" "fetch" "charset" "logs" "errors" "cursor" "view" "ps" 
  "ps_bugs" "sp" "result" "connection" "misc" )

foreach ( API_TEST ${API_TESTS} )
add_executable ( ${API_TEST} ${API_TEST}.c )
target_link_libraries ( ${API_TEST} mytap mysql ${WIN32_ADD_LIBRARIES} )
add_test ( ${API_TEST} ${EXECUTABLE_OUTPUT_PATH}/${API_TEST} )
set_tests_properties ( ${API_TEST} PROPERTIES TIMEOUT 120 )
endforeach ( API_TEST )
