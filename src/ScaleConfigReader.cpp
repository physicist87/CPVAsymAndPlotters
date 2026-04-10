#include "../include/ScaleConfigReader.hpp"

ScaleConfigReader::ScaleConfigReader()
    : dy_sf_(1.0), dy_sf_found_(false) {}

ScaleConfigReader::~ScaleConfigReader() {}

std::string ScaleConfigReader::trim(const std::string& s) const {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string ScaleConfigReader::stripComment(const std::string& line) const {
    size_t pos = line.find('#');
    if (pos != std::string::npos) return line.substr(0, pos);
    return line;
}

bool ScaleConfigReader::load(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "ScaleConfigReader: Cannot open " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(f, line)) {
        line = trim(stripComment(line));
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "DY") {
            // Step-wise DY format: DY  <step|*>  <SF>
            std::string step;
            double sf;
            if (!(ss >> step >> sf)) continue;

            if (step == "*") {
                dy_sf_ = sf;
                dy_sf_found_ = true;
            }
            // other step entries (0~9) are parsed but not stored
        } else {
            // Flat format: <SampleName>  <SF>
            double sf;
            if (!(ss >> sf)) continue;
            sample_sf_[token] = sf;
        }
    }

    f.close();
    return true;
}

double ScaleConfigReader::getSF(const std::string& sampleName) const {
    auto it = sample_sf_.find(sampleName);
    if (it != sample_sf_.end()) return it->second;
    return 1.0;
}

double ScaleConfigReader::getDYSF() const {
    return dy_sf_;
}

void ScaleConfigReader::printSummary() const {
    std::cout << "[ScaleConfigReader]" << std::endl;
    if (dy_sf_found_) {
        std::cout << "  DY SF (*): " << dy_sf_ << std::endl;
    } else {
        std::cout << "  DY SF (*): not found, using 1.0" << std::endl;
    }
    for (const auto& p : sample_sf_) {
        if (p.second != 1.0) {
            std::cout << "  " << p.first << " : " << p.second << std::endl;
        }
    }
}
