# Copyright (c) 2013 Louis Henry Nayegon.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# The software should be used for Good, not Evil.

cmake_minimum_required (VERSION 2.8)
project (VIA-HTTPLIB)

option( VIA_HTTPLIB_BUILD_SHARED_LIBS "Build via-httplib as shared libraries." OFF )
option( VIA_HTTPLIB_BUILD_TESTS "Build the unit tests." ON )

set( VIA_HTTPLIB_LIBRARY_TYPE STATIC )
set( VIA_HTTPLIB_LIBRARY_NAME via-httplib )

find_package(OpenSSL)
find_package(Threads)

if (OPENSSL_FOUND)
    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
endif()

if (MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)
if (WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

include_directories(${BOOST_INCLUDEDIRS})

add_library( ${VIA_HTTPLIB_LIBRARY_NAME} ${VIA_HTTPLIB_LIBRARY_TYPE}
  via/http/character.cpp
  via/http/chunk.cpp
  via/http/header_field.cpp
  via/http/headers.cpp
  via/http/request.cpp
  via/http/request_method.cpp
  via/http/response.cpp
  via/http/response_status.cpp
)
