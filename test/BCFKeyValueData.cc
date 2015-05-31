#include <iostream>
#include "BCFKeyValueData.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("BCFKeyValueData construction") {
	using namespace GLnexus;
	unique_ptr<BCFKeyValueData> data;
	REQUIRE(BCFKeyValueData::Open(nullptr, data).ok());
}
