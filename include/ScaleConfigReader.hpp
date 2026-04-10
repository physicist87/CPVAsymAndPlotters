#ifndef SCALECONFIGREADER_HPP
#define SCALECONFIGREADER_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

// Parses ScaleConfig files in two formats:
//   Flat:      <SampleName>  <SF>
//   Step-wise: DY  <step|*>  <SF>   [# optional comment]
//
// For DY, only the "*" (suffix-less) entry is used,
// which corresponds to the final b-tag selection step.

class ScaleConfigReader {
public:
    ScaleConfigReader();
    ~ScaleConfigReader();

    bool load(const std::string& filepath);

    // Returns SF for a non-DY sample. Returns 1.0 if not found.
    double getSF(const std::string& sampleName) const;

    // Returns DY SF (the "*" entry). Returns 1.0 if not found.
    double getDYSF() const;

    void printSummary() const;

private:
    std::map<std::string, double> sample_sf_;
    double dy_sf_;
    bool dy_sf_found_;

    std::string stripComment(const std::string& line) const;
    std::string trim(const std::string& s) const;
};

#endif
