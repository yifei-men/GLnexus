#ifndef GLNEXUS_BCFKEYVALUEDATA_H
#define GLNEXUS_BCFKEYVALUEDATA_H

#include "data.h"

namespace GLnexus {

/// Abstract interface to a key-value database underlying BCFKeyValueData
namespace KeyValue {

/// The DB is partitioned into collections of keys and values. This is an
/// abstract handle to a collection.
class Collection;

/*
class Iterator {
public:
    virtual Status next(std::pair<rocksdb::Slice,rocksdb::Slice>& entry) = 0;
};
*/

/// A DB snapshot, from which consistent multiple reads may be taken
class Snapshot {
public:
    virtual Status get(Collection* coll, const std::string& key, std::string& value) = 0;
    //virtual Status iterator(Collection* coll, const std::string& key, std::unique_ptr<Iterator>& it) = 0;
};

/// A batch of writes to apply atomically
class Writes {
public:
    virtual Status put(Collection* coll, const std::string& key, const std::string& value) = 0;
    //virtual Status delete(Collection* coll, const std::string& key) = 0;
};

/// Main database interface for retrieving collection handles, generating
/// snapshopts to read from, and creating and applying write batches. The DB
/// object itself implements the Snapshot interface (with no consistency
/// guarantees between multiple calls) and the Writes intreface (with no
/// atomicity guarantees between multiple calls). Caller must ensure that the
/// parent DB object must still exists when any Snapshot or Writes object is
/// used.
class DB : public Snapshot, public Writes {
public:
    virtual Status collection(const std::string& name, Collection** coll) = 0;
    virtual Status current(std::unique_ptr<Snapshot>& snapshot) = 0;
    virtual Status open_writes(std::unique_ptr<Writes>& writes) = 0;
    virtual Status apply_writes(Writes* writes) = 0;
};

}

/// An implementation of the Data interface that stores sample sets and BCF
/// records in a given key-value database. One imported gVCF file (potentially
/// with multiple samples) becomes a data set. The key schema permits
/// efficient retrieval by genomic range across the datasets.
class BCFKeyValueData : public Data {
    struct body;
    std::unique_ptr<body> body_;

    BCFKeyValueData();

public:
    static Status InitializeDB(KeyValue::DB* db, const std::vector<std::pair<std::string,size_t> >& contigs);
    static Status Open(KeyValue::DB* db, std::unique_ptr<BCFKeyValueData>& ans);
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
