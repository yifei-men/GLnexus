#include "BCFKeyValueData.h"
#include "yaml-cpp/yaml.h"
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
using namespace std;

namespace GLnexus {

// pImpl idiom
template<class KeyValueDB>
struct BCFKeyValueData<KeyValueDB>::body {
    KeyValueDB* db;
};

template<class KeyValueDB> BCFKeyValueData<KeyValueDB>::BCFKeyValueData() = default;
template<class KeyValueDB> BCFKeyValueData<KeyValueDB>::~BCFKeyValueData() = default;

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::InitializeDB(KeyValueDB* db, const vector<pair<string,size_t>>& contigs) {
    Status s;
    S(db->create_collection("config"));
    S(db->create_collection("headers"));
    S(db->create_collection("records"));
    typename KeyValueDB::collection_handle_type config;
    S(db->collection("config", config));
    set<string> prev_contigs;
    YAML::Emitter yaml;
    yaml << YAML::BeginSeq;
    for (const auto& p : contigs) {
        if (prev_contigs.find(p.first) != prev_contigs.end()) {
            return Status::Invalid("duplicate reference contig", p.first);
        }
        yaml << YAML::BeginMap;
        yaml << YAML::Key << p.first;
        yaml << YAML::Value << p.second;
        yaml << YAML::EndMap;
    }
    yaml << YAML::EndSeq;
    return db->put(config, "contigs", yaml.c_str());
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::Open(KeyValueDB* db, unique_ptr<BCFKeyValueData<KeyValueDB>>& ans) {
    assert(db != nullptr);
    ans.reset(new BCFKeyValueData<KeyValueDB>());
    ans->body_.reset(new body);
    ans->body_->db = db;
    
    typename KeyValueDB::collection_handle_type coll;
    if (db->collection("config", coll).bad() ||
        db->collection("headers", coll).bad() ||
        db->collection("records", coll).bad()) {
        return Status::Invalid("database hasn't been properly initialized");
    }

    return Status::OK();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::contigs(vector<pair<string,size_t> >& ans) const {
    Status s;
    typename KeyValueDB::collection_handle_type coll;
    S(body_->db->collection("config", coll));

    const char *unexpected = "BCFKeyValueData::contigs unexpected YAML";
    ans.clear();
    try {
        string contigs_yaml;
        S(body_->db->get(coll, "contigs", contigs_yaml));
        YAML::Node n = YAML::Load(contigs_yaml);
        if (!n.IsSequence()) {
            return Status::Invalid(unexpected, contigs_yaml);
        }
        for (const auto& item : n) {
            if (!item.IsMap() || item.size() != 1) {
                return Status::Invalid(unexpected, contigs_yaml);
            }
            auto m = item.as<map<string,size_t>>();
            assert (m.size() == 1);
            ans.push_back(*(m.begin()));
        }
    } catch(YAML::Exception& exn) {
        return Status::Invalid("BCFKeyValueData::contigs YAML parse error", exn.msg);
    }
    if (ans.empty()) {
        return Status::Invalid("database has empty contigs metadata");
    }

    return Status::OK();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::sampleset_samples(const string& sampleset,
                                          shared_ptr<const set<string> >& ans) const {
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::sample_dataset(const string& sampleset, string& ans) const {
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::dataset_bcf_header(const string& dataset,
                                           shared_ptr<const bcf_hdr_t>& hdr) const {
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::dataset_bcf(const string& dataset, const range& pos,
                                    shared_ptr<const bcf_hdr_t>& hdr,
                                    vector<shared_ptr<bcf1_t> >& records) const {
    records.clear();
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::import_gvcf(const string& dataset, const string& filename) {
    return Status::NotImplemented();
}

template class BCFKeyValueData<KeyValue::Mem::DB>;

} // namespace GLnexus
