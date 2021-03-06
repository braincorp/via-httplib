//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Via Technology Ltd. All Rights Reserved.
// (ken dot barker at via-technology dot co dot uk)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////
#include "via/http/chunk.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>
#include <iostream>

using namespace via::http;

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestChunkLineParser)

BOOST_AUTO_TEST_CASE(EmptyChunk1)
{
  std::string chunk_data("0\r\n");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() == next);
  BOOST_CHECK_EQUAL("0",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL("", the_chunk.extension().c_str());
  BOOST_CHECK_EQUAL(0, the_chunk.size());
  BOOST_CHECK(the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(EmptyChunk2)
{
  std::string chunk_data("0;\r\n");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() == next);
  BOOST_CHECK_EQUAL("0",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL("", the_chunk.extension().c_str());
  BOOST_CHECK_EQUAL(0, the_chunk.size());
  BOOST_CHECK(the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(ValidString1)
{
  std::string chunk_data("f; some rubbish\r\n");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() == next);
  BOOST_CHECK_EQUAL("f",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL("some rubbish", the_chunk.extension().c_str());
  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK(!the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(ValidString2)
{
  std::string chunk_data("f\r\nA");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() != next);
  BOOST_CHECK_EQUAL('A', *next);
  BOOST_CHECK_EQUAL("f",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK(!the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(ValidString3)
{
  std::string chunk_data("f; some rubbish\r\nA");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() != next);
  BOOST_CHECK_EQUAL('A', *next);
  BOOST_CHECK_EQUAL("f",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL("some rubbish", the_chunk.extension().c_str());
  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK(!the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(MultipleString1)
{
  std::string chunk_data("2f; some rubbish\r\n");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(!the_chunk.parse(next, chunk_data.cbegin() +1));
  BOOST_CHECK(chunk_data.end() != next);
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK(chunk_data.end() == next);
  BOOST_CHECK_EQUAL("2f",  the_chunk.hex_size().c_str());
  BOOST_CHECK_EQUAL("some rubbish", the_chunk.extension().c_str());
  BOOST_CHECK_EQUAL(47, the_chunk.size());
}

BOOST_AUTO_TEST_CASE(InValidString1)
{
  std::string chunk_data("g;\r\n");
  auto next(chunk_data.cbegin());

  chunk_header the_chunk;
  BOOST_CHECK(!the_chunk.parse(next, chunk_data.cend()));
}

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestChunkEncoder)

BOOST_AUTO_TEST_CASE(EmptyChunk1)
{
  chunk_header the_chunk(0);
  auto chunk_string(the_chunk.to_string());

  BOOST_CHECK_EQUAL("0\r\n",  chunk_string.c_str());
}

BOOST_AUTO_TEST_CASE(ValidChunk1)
{
  chunk_header the_chunk(15);
  auto chunk_string(the_chunk.to_string());

  BOOST_CHECK_EQUAL("f\r\n",  chunk_string.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestLastChunkEncoder)

BOOST_AUTO_TEST_CASE(EmptyChunk1)
{
  std::string empty_string("");
  last_chunk the_chunk(empty_string, empty_string);
  auto chunk_string(the_chunk.message());

  BOOST_CHECK_EQUAL("0\r\n\r\n", chunk_string.c_str());
}

BOOST_AUTO_TEST_CASE(EmptyChunk2)
{
  std::string empty_string("");
  last_chunk the_chunk("extension", empty_string);
  auto chunk_string(the_chunk.message());

  BOOST_CHECK_EQUAL("0; extension\r\n\r\n", chunk_string.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestChunkParser)

BOOST_AUTO_TEST_CASE(ValidChunk1)
{
  std::string chunk_data("f;\r\n");
  chunk_data += "123456789abcdef\r\n";
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK_EQUAL('1', the_chunk.data().front());
  BOOST_CHECK_EQUAL('f', the_chunk.data().back());
}

BOOST_AUTO_TEST_CASE(ValidChunk2)
{
  std::string chunk_data("f;\n");
  chunk_data += "123456789abcdef\n";
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK_EQUAL('1', the_chunk.data().front());
  BOOST_CHECK_EQUAL('f', the_chunk.data().back());
}

BOOST_AUTO_TEST_CASE(ValidChunk3)
{
  std::string chunk_data("f;\r\n");
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(!the_chunk.parse(next, chunk_data.cend()));

  std::string chunk_data1("123456789abcdef\r\n");
  next = chunk_data1.begin();
  BOOST_CHECK(the_chunk.parse(next, chunk_data1.cend()));

  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK_EQUAL('1', the_chunk.data().front());
  BOOST_CHECK_EQUAL('f', the_chunk.data().back());
}

BOOST_AUTO_TEST_CASE(ValidChunk4)
{
  std::string chunk_data("f");
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(!the_chunk.parse(next, chunk_data.cend()));

  std::string chunk_data1(";\r\n123456789abcdef\r\n");
  next = chunk_data1.begin();
  BOOST_CHECK(the_chunk.parse(next, chunk_data1.cend()));

  BOOST_CHECK_EQUAL(15, the_chunk.size());
  BOOST_CHECK_EQUAL('1', the_chunk.data().front());
  BOOST_CHECK_EQUAL('f', the_chunk.data().back());
}

BOOST_AUTO_TEST_CASE(ValidLastChunk1)
{
  std::string chunk_data("0\r\n\r\n");
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK_EQUAL(0, the_chunk.size());
  BOOST_CHECK(the_chunk.valid());
  BOOST_CHECK(the_chunk.is_last());
}

BOOST_AUTO_TEST_CASE(ValidChunkTrailer1)
{
  std::string chunk_data("0\r\n");
  chunk_data += "Accept-Encoding: gzip\r\n\r\n";
  auto next(chunk_data.cbegin());

  rx_chunk<std::string> the_chunk;
  BOOST_CHECK(the_chunk.parse(next, chunk_data.cend()));
  BOOST_CHECK_EQUAL(0, the_chunk.size());
  BOOST_CHECK(the_chunk.is_last());
}

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////
