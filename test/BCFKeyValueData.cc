#include <iostream>
#include <map>
#include "BCFKeyValueData.h"
#include "catch.hpp"
using namespace std;
using namespace GLnexus;

using T = BCFKeyValueData<KeyValue::Mem::DB>;

TEST_CASE("BCFKeyValueData construction on improperly initialized database") {
    vector<string> collections = {"headers","records"};
    KeyValue::Mem::DB db(collections);
    unique_ptr<T> data;
    REQUIRE(T::Open(&db, data) == StatusCode::INVALID);
}

TEST_CASE("BCFKeyValueData initialization") {
    KeyValue::Mem::DB db({});
    auto contigs = {make_pair<string,uint64_t>("21", 1000000), make_pair<string,uint64_t>("22", 1000001)};
    REQUIRE(T::InitializeDB(&db, contigs).ok());
    unique_ptr<T> data;
    REQUIRE(T::Open(&db, data).ok());

    SECTION("contigs") {
        vector<pair<string,size_t>> contigs;
        REQUIRE(data->contigs(contigs).ok());
        REQUIRE(contigs.size() == 2);
        REQUIRE(contigs[0].first == "21");
        REQUIRE(contigs[0].second == 1000000);
        REQUIRE(contigs[1].first == "22");
        REQUIRE(contigs[1].second == 1000001);
    }
}

