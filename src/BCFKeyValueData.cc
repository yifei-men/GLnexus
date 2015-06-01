#include "BCFKeyValueData.h"
#include <assert.h>
using namespace std;

namespace GLnexus {

namespace KeyValue {

    template<typename CollectionHandle, class ReaderImpl, class WriteBatchImpl>
    Status DB<CollectionHandle,ReaderImpl,WriteBatchImpl>::get(const CollectionHandle& coll, const std::string& key, std::string& value) const {
        Status s;
        unique_ptr<ReaderImpl> curr;
        S(current(curr));
        return curr->get(coll, key, value);
    }

    template<typename CollectionHandle, class ReaderImpl, class WriteBatchImpl>
    Status DB<CollectionHandle,ReaderImpl,WriteBatchImpl>::put(const CollectionHandle& coll, const std::string& key, const std::string& value) {
        Status s;
        unique_ptr<WriteBatchImpl> batch;
        S(begin_writes(batch));
        assert(batch);
        S(batch->put(coll, key, value));
        return commit_writes(batch.get());
    }

    template Status DB<uint64_t,Mem::Reader,Mem::WriteBatch>::get(const uint64_t& coll, const std::string& key, std::string& value) const;
    template Status DB<uint64_t,Mem::Reader,Mem::WriteBatch>::put(const uint64_t& coll, const std::string& key, const std::string& value);


} // namespace KeyValue

template<class KeyValueDB>
struct BCFKeyValueData<KeyValueDB>::body {
    KeyValueDB* db;
};

template<class KeyValueDB> BCFKeyValueData<KeyValueDB>::BCFKeyValueData() = default;
template<class KeyValueDB> BCFKeyValueData<KeyValueDB>::~BCFKeyValueData() = default;

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::InitializeDB(KeyValueDB* db, const vector<pair<string,size_t>>& contigs) {
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::Open(KeyValueDB* db, unique_ptr<BCFKeyValueData<KeyValueDB>>& ans) {
    assert(db != nullptr);
    ans.reset(new BCFKeyValueData<KeyValueDB>());
    ans->body_.reset(new body);
    ans->body_->db = db;
    // TODO check existence of requisite collections...
    return Status::OK();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::contigs(vector<pair<string,size_t> >& ans) const {
    return Status::NotImplemented();
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
    return Status::NotImplemented();
}

template<class KeyValueDB>
Status BCFKeyValueData<KeyValueDB>::import_gvcf(const string& dataset, const string& filename) {
    return Status::NotImplemented();
}

template class BCFKeyValueData<KeyValue::Mem::DB>;

} // namespace GLnexus
