# Copyright (C) 2007 MySQL AB
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

include_directories ( ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/zlib ${CMAKE_SOURCE_DIR}/sql 
  ${CMAKE_SOURCE_DIR}/regex ${CMAKE_SOURCE_DIR}/extra/yassl/include ${CMAKE_SOURCE_DIR}/unittest/mytap )

add_executable ( simple-t simple-t.c )
target_link_libraries ( simple-t mytap ${WIN32_ADD_LIBRARIES} )

add_executable ( skip-t skip-t.c )
target_link_libraries ( skip-t mytap ${WIN32_ADD_LIBRARIES} )

add_executable ( todo-t todo-t.c )
target_link_libraries ( todo-t mytap ${WIN32_ADD_LIBRARIES} )

add_executable ( skip_all-t skip_all-t.c )
target_link_libraries ( skip_all-t mytap ${WIN32_ADD_LIBRARIES} )

add_executable ( no_plan-t no_plan-t.c )
target_link_libraries ( no_plan-t mytap ${WIN32_ADD_LIBRARIES} )

add_executable ( core-t core-t.c )
target_link_libraries ( core-t mytap ${WIN32_ADD_LIBRARIES} )
