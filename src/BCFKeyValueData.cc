#include "BCFKeyValueData.h"
using namespace std;

namespace GLnexus {

namespace KeyValue {

} // namespace KeyValue

struct BCFKeyValueData::body {
    KeyValue::DB* db;
};

BCFKeyValueData::BCFKeyValueData() = default;
BCFKeyValueData::~BCFKeyValueData() = default;

Status BCFKeyValueData::InitializeDB(KeyValue::DB* db, const vector<pair<string,size_t>>& contigs) {
    return Status::NotImplemented();
}

Status BCFKeyValueData::Open(KeyValue::DB* db, unique_ptr<BCFKeyValueData>& ans) {
    ans.reset(new BCFKeyValueData());
    ans->body_.reset(new body);
    ans->body_->db = db;
    // TODO check existence of requisite collections...
    return Status::OK();
}

Status BCFKeyValueData::contigs(vector<pair<string,size_t> >& ans) const {
    return Status::NotImplemented();
}

Status BCFKeyValueData::sampleset_samples(const string& sampleset,
                                          shared_ptr<const set<string> >& ans) const {
    return Status::NotImplemented();
}

Status BCFKeyValueData::sample_dataset(const string& sampleset, string& ans) const {
    return Status::NotImplemented();
}

Status BCFKeyValueData::dataset_bcf_header(const string& dataset,
                                           shared_ptr<const bcf_hdr_t>& hdr) const {
    return Status::NotImplemented();
}

Status BCFKeyValueData::dataset_bcf(const string& dataset, const range& pos,
                                    shared_ptr<const bcf_hdr_t>& hdr,
                                    vector<shared_ptr<bcf1_t> >& records) const {
    return Status::NotImplemented();
}

Status BCFKeyValueData::import_gvcf(const string& dataset, const string& filename) {
    return Status::NotImplemented();
}

} // namespace GLnexus
