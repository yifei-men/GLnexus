#include "BCFKeyValueData.h"
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
    int i = 0;
    auto w = ceil(log10(contigs.size()));
    for (const auto& p : contigs) {
        if (prev_contigs.find(p.first) != prev_contigs.end()) {
            return Status::Invalid("duplicate reference contig", p.first);
        }
        ostringstream key;
        key << "ctg:";
        key << setw(w) << setfill('0') << i++;
        key << ':';
        key << p.first;
        S(db->put(config, key.str(), to_string(p.second)));
        prev_contigs.insert(p.first);
    }
    return Status::OK();
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
    unique_ptr<typename KeyValueDB::iterator_type> it;
    string key, value;

    S(body_->db->collection("config", coll));
    S(body_->db->iterator(coll, "ctg:", it));

    ans.clear();
    while ((s = it->next(key, value)).ok() && key.size() > 4 && key.substr(0, 4) == "ctg:") {
        char *contig = strchr((char*)key.c_str() + 4, ':');
        if (contig == nullptr || *(++contig) == 0) {
            return Status::Invalid("corrupt database contig metadata (key name)", key);
        }
        size_t sz;
        try {
            sz = stoul(value);
        } catch (...) {
            return Status::Invalid("corrupt database contig metadata (contig length)", key);
        }
        ans.push_back(make_pair(string(contig), sz));
    }
    if (s.bad() && s != StatusCode::NOT_FOUND) return s;
    if (ans.empty()) {
        return Status::Invalid("database missing contig metadata");
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
