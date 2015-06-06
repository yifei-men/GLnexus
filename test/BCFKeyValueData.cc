#include <iostream>
#include <map>
#include "BCFKeyValueData.h"
#include "catch.hpp"
using namespace std;
using namespace GLnexus;

using T = BCFKeyValueData<KeyValue::Mem::DB>;

TEST_CASE("BCFKeyValueData construction on improperly initialized database") {
    vector<string> collections = {"header","bcf"};
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
        Status s = data->contigs(contigs);
        REQUIRE(s.ok());
        REQUIRE(contigs.size() == 2);
        REQUIRE(contigs[0].first == "21");
        REQUIRE(contigs[0].second == 1000000);
        REQUIRE(contigs[1].first == "22");
        REQUIRE(contigs[1].second == 1000001);
    }

    SECTION("sampleset_samples") {
        typename KeyValue::Mem::DB::collection_handle_type coll;
        string null(1, '\0');
        REQUIRE(db.collection("sampleset", coll).ok());
        REQUIRE(db.put(coll, "trio1", "").ok());
        REQUIRE(db.put(coll, "trio1" + null + "fa", "").ok());
        REQUIRE(db.put(coll, "trio1" + null + "mo", "").ok());
        REQUIRE(db.put(coll, "trio1" + null + "ch", "").ok());
        REQUIRE(db.put(coll, "trio2", "").ok());
        REQUIRE(db.put(coll, "trio2" + null + "fa2", "").ok());
        REQUIRE(db.put(coll, "trio2" + null + "mo2", "").ok());
        REQUIRE(db.put(coll, "trio2" + null + "ch2", "").ok());

        shared_ptr<const set<string>> samples;
        REQUIRE(data->sampleset_samples("trio1", samples).ok());
        REQUIRE(samples->size() == 3);
        REQUIRE(samples->find("fa") != samples->end());
        REQUIRE(samples->find("mo") != samples->end());
        REQUIRE(samples->find("ch") != samples->end());
        REQUIRE(samples->find("fa2") == samples->end());

        REQUIRE(data->sampleset_samples("trio2", samples).ok());
        REQUIRE(samples->size() == 3);
        REQUIRE(samples->find("fa2") != samples->end());
        REQUIRE(samples->find("mo2") != samples->end());
        REQUIRE(samples->find("ch2") != samples->end());
        REQUIRE(samples->find("fa") == samples->end());

        REQUIRE(data->sampleset_samples("bogus", samples) == StatusCode::NOT_FOUND);
    }

    SECTION("sample_dataset") {
        typename KeyValue::Mem::DB::collection_handle_type coll;
        REQUIRE(db.collection("sample_dataset", coll).ok());
        REQUIRE(db.put(coll, "fa", "trio1").ok());
        REQUIRE(db.put(coll, "mo", "trio1").ok());
        REQUIRE(db.put(coll, "ch", "trio1").ok());
        REQUIRE(db.put(coll, "fa2", "trio2").ok());
        REQUIRE(db.put(coll, "mo2", "trio2").ok());
        REQUIRE(db.put(coll, "ch2", "trio2").ok());

        string dataset;
        REQUIRE(data->sample_dataset("fa", dataset).ok());
        REQUIRE(dataset == "trio1");
        REQUIRE(data->sample_dataset("ch", dataset).ok());
        REQUIRE(dataset == "trio1");
        REQUIRE(data->sample_dataset("mo2", dataset).ok());
        REQUIRE(dataset == "trio2");
        REQUIRE(data->sample_dataset("bogus", dataset) == StatusCode::NOT_FOUND);
    }
}

TEST_CASE("BCFKeyValueData::import_gvcf") {
    KeyValue::Mem::DB db({});
    auto contigs = {make_pair<string,uint64_t>("21", 1000000), make_pair<string,uint64_t>("22", 1000001)};
    REQUIRE(T::InitializeDB(&db, contigs).ok());
    unique_ptr<T> data;
    REQUIRE(T::Open(&db, data).ok());

    SECTION("NA12878D_HiSeqX.21.10009462-10009469.gvcf") {
        Status s = data->import_gvcf("NA12878D", "test/data/NA12878D_HiSeqX.21.10009462-10009469.gvcf");
        REQUIRE(s.ok());

        string dataset;
        REQUIRE(data->sample_dataset("NA12878", dataset).ok());
        REQUIRE(dataset == "NA12878D");
    }

    // TODO test importing a gVCF with mismatching contigs
}