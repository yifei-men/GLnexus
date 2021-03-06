#ifndef GLNEXUS_SERVICE_H
#define GLNEXUS_SERVICE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "types.h"
#include "data.h"

namespace GLnexus {

class Service {
    std::unique_ptr<DataCache> data_;
    std::vector<std::pair<std::string,size_t> > contigs_;

    Service() {}

    Status sampleset_datasets(const std::string& sampleset, std::shared_ptr<const std::set<std::string>>& ans);

public:
    static Status Start(Data* data, std::unique_ptr<Service>& svc);


    /// Discover all the alleles overlapping the given range for a sample set.

    /// Each allele has a reference range and DNA sequence. Additionally, it's
    /// marked as REF or not, and includes an estimate of the observation
    /// count in the sample set, which may be useful for making an empirical
    /// estimate of the allele frequency. The discovered alleles may be
    /// overlapping; the set is generally used as input to allele
    /// "unification"
    Status discover_alleles(const std::string& sampleset, const range& pos, discovered_alleles& ans);


    /// Genotype a set of samples at the given sites, producing a BCF file.
    Status genotype_sites(const std::string& sampleset, const std::vector<unified_site>& sites, const std::string& filename);
};

}

#endif
