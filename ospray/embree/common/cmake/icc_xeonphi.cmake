## ======================================================================== ##
## Copyright 2009-2015 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

SET (CMAKE_CXX_COMPILER icpc)
SET (CMAKE_CXX_FLAGS "-mmic -restrict -Wall -fasm-blocks -fPIC")
SET (CMAKE_CXX_FLAGS_NOOPT "-g -O0 -DDEBUG")
SET (CMAKE_CXX_FLAGS_DEBUG "-g -w1 -O2 -DDEBUG ")

#--no-exceptions --no-rtti  
# -ansi-alias # ISPC not working with ansi-alias flag

SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt  -mGLOB_default_function_attrs=\"use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on\"  -mP2OPT_hlo_prefetch=F ")

SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3 -DNDEBUG -fp-model fast -fimf-precision=low -fasm-blocks -no-inline-max-total-size -inline-factor=200 -fPIC  -fma  -restrict -no-prec-div -no-prec-sqrt  -mGLOB_default_function_attrs=\"use_vec_for_imul=on;use_fast_math=on;gather_scatter_loop_jknzd=on;gather_scatter_loop_unroll=2;use_gather_scatter_hint=on;c_sch_nop_insertion=on;c_avoid_movz_and_movs=off;c_avoid_integer_ciscization=on;avoid_long_vector_ints=on;avoid_loads_with_extend=on;smart_mem_conflicts=on\"  -mP2OPT_hlo_prefetch=F ")

SET(CMAKE_LINKER icpc)
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-intel")

#SET(CMAKE_CXX_LINK_EXECUTABLE  	    "<CMAKE_CXX_COMPILER> -mmic -static-intel -rdynamic -fPIC -L${CMAKE_BINARY_DIR} <LINK_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES>")
#SET(CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> -mmic -static-intel  <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_MODULE_CXX_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES> -zmuldefs")
