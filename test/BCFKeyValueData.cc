#include <iostream>
#include <map>
#include "BCFKeyValueData.h"
#include "catch.hpp"
using namespace std;
using namespace GLnexus;

TEST_CASE("BCFKeyValueData construction") {
	vector<string> collections = {"metadata","bcf"};
	KeyValue::Mem::DB db(collections);
	using T = BCFKeyValueData<KeyValue::Mem::DB>;
	unique_ptr<T> data;
	REQUIRE(T::Open(&db, data).ok());
}

// test open on empty database...
