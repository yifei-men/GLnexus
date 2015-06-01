#ifndef GLNEXUS_BCFKEYVALUEDATA_H
#define GLNEXUS_BCFKEYVALUEDATA_H

#include "data.h"

namespace GLnexus {

/// Abstract interface to a key-value database underlying BCFKeyValueData
namespace KeyValue {

/*
class Iterator {
public:
    virtual Status next(std::pair<rocksdb::Slice,rocksdb::Slice>& entry) = 0;
};
*/

/// A DB snapshot, from which consistent multiple reads may be taken
template<typename CollectionHandle>
class Reader {
public:
    virtual Status get(const CollectionHandle& coll, const std::string& key, std::string& value) const = 0;
    //virtual Status iterator(Collection* coll, const std::string& key, std::unique_ptr<Iterator>& it) = 0;
};

/// A batch of writes to apply atomically
template<typename CollectionHandle>
class WriteBatch {
public:
    virtual Status put(const CollectionHandle& coll, const std::string& key, const std::string& value) = 0;
    //virtual Status delete(Collection* coll, const std::string& key) = 0;
};

/// Main database interface for retrieving collection handles, generating
/// snapshopts to read from, and creating and applying write batches. The DB
/// object itself implements the Reader interface (with no consistency
/// guarantees between multiple calls) and the WriteBatch interface (which
/// applies one write immediately, no atomicity guarantees between multiple
/// calls). Caller must ensure that the parent DB object still exists when any
/// Reader or WriteBatch object is used.
template<typename CollectionHandle, class ReaderImpl, class WriteBatchImpl>
class DB : public ReaderImpl, public WriteBatchImpl {
public:
    static_assert(std::is_base_of<Reader<CollectionHandle>, ReaderImpl>::value, "ReaderImpl must implement Reader interface");
    static_assert(std::is_base_of<WriteBatch<CollectionHandle>, WriteBatchImpl>::value, "WriterImpl must implement Writer interface");
    virtual Status collection(const std::string& name, CollectionHandle& coll) const = 0;
    virtual Status current(std::unique_ptr<ReaderImpl>& snapshot) const = 0;
    virtual Status begin_writes(std::unique_ptr<WriteBatchImpl>& writes) = 0;
    virtual Status commit_writes(WriteBatchImpl* writes) = 0;

    Status get(const CollectionHandle& coll, const std::string& key, std::string& value) const override;
    Status put(const CollectionHandle& coll, const std::string& key, const std::string& value) override;
};

// trivial in-memory KeyValue::DB implementation
namespace Mem {
    class Reader : public KeyValue::Reader<uint64_t> {
        std::vector<std::map<std::string,std::string>> data_;
        friend class DB;

    public:
        Status get(const uint64_t& coll, const std::string& key, std::string& value) const override {
            assert(coll < data_.size());
            const auto& m = data_[coll];
            auto p = m.find(key);
            if (p == m.end()) return Status::NotFound("key", key);
            value = p->second;
            return Status::OK();
        }
    };

    class WriteBatch : public KeyValue::WriteBatch<uint64_t> {
        std::vector<std::map<std::string,std::string>> data_;
        friend class DB;

    public:
        Status put(const uint64_t& coll, const std::string& key, const std::string& value) override {
            assert(coll < data_.size());
            data_[coll][key] = value;
            return Status::OK();
        };
    };

    class DB : public KeyValue::DB<uint64_t,Mem::Reader,Mem::WriteBatch> {
        std::map<std::string,uint64_t> collections_;
        std::vector<std::map<std::string,std::string>> data_;

    public:
        DB(const std::vector<std::string>& collections) {
            for (uint64_t i = 0; i < collections.size(); i++) {
                assert(collections_.find(collections[i]) == collections_.end());
                collections_[collections[i]] = i;
            }
            data_ = std::vector<std::map<std::string,std::string>>(collections_.size());
        }

        Status collection(const std::string& name, uint64_t& coll) const override {
            auto p = collections_.find(name);
            if (p == collections_.end()) return Status::NotFound("KeyValue::Mem::collection", name);
            coll = p->second;
            return Status::OK();
        }

        Status current(std::unique_ptr<Mem::Reader>& reader) const override {
            auto p = std::make_unique<KeyValue::Mem::Reader>();
            p->data_ = data_;
            reader = std::move(p);
            return Status::OK();
        }

        Status begin_writes(std::unique_ptr<Mem::WriteBatch>& writes) override {
            auto p = std::make_unique<Mem::WriteBatch>();
            p->data_ = std::vector<std::map<std::string,std::string>>(data_.size());
            writes = std::move(p);
            return Status::OK();
        }

        Status commit_writes(Mem::WriteBatch* writes) override {
            assert(writes != nullptr);
            assert(writes->data_.size() <= data_.size());
            for (size_t i = 0; i < writes->data_.size(); i++) {
                for (const auto& p : writes->data_[i]) {
                    data_[i][p.first] = p.second;
                }
            }
            return Status::OK();
        }
    };
}

}




/// An implementation of the Data interface that stores sample sets and BCF
/// records in a given key-value database. One imported gVCF file (potentially
/// with multiple samples) becomes a data set. The key schema permits
/// efficient retrieval by genomic range across the datasets.
template<class KeyValueDB>
class BCFKeyValueData : public Data {
    struct body;
    std::unique_ptr<body> body_;

    BCFKeyValueData();

public:
    static Status InitializeDB(KeyValueDB* db, const std::vector<std::pair<std::string,size_t> >& contigs);
    static Status Open(KeyValueDB* db, std::unique_ptr<BCFKeyValueData<KeyValueDB>>& ans);
    ~BCFKeyValueData();

    Status contigs(std::vector<std::pair<std::string,size_t> >& ans) const override;
    Status sampleset_samples(const std::string& sampleset,
                             std::shared_ptr<const std::set<std::string> >& ans) const override;
    Status sample_dataset(const std::string& sampleset, std::string& ans) const override;
    
    Status dataset_bcf_header(const std::string& dataset,
                              std::shared_ptr<const bcf_hdr_t>& hdr) const override;
    Status dataset_bcf(const std::string& dataset, const range& pos,
                       std::shared_ptr<const bcf_hdr_t>& hdr,
                       std::vector<std::shared_ptr<bcf1_t> >& records) const override;


    Status import_gvcf(const std::string& dataset, const std::string& filename);
};

}

#endif
