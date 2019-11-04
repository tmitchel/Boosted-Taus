// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_HISTMANAGER_H_
#define INTERFACE_HISTMANAGER_H_

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "./json.hpp"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// histManager holds a map of histograms and provides
// methods to fill them. Histograms are ready from the
// provided json file which includes a histogram name
// and corresponding binning. histManager also allows all
// histograms to be written at one time.
class histManager {
   public:
    explicit histManager(std::string);
    ~histManager() {}
    void load_histograms(std::string);
    void Fill(std::string, double, double);
    void Fill2d(std::string, double, double, double);
    void FillBin(std::string, int, double);
    void FillPrevBins(std::string, double, double);
    void Write();

    // I'll make this public in case you want to handle
    // things directly. (Don't make me regret it!)
    std::map<std::string, TH1F*> hists_1d;
    std::map<std::string, TH2F*> hists_2d;
    std::shared_ptr<TFile> fout;
};

// When constructing a histManager, create the output file
// with the given name.
histManager::histManager(std::string fname) : fout(std::make_shared<TFile>(fname.c_str(), "RECREATE")) {
    fout->cd();
    hists_1d = {};  // you can hardcode histograms here
    hists_2d = {};  // you can hardcode histograms here
}

// load_histograms reads the provided json config file
// and then creates the requested histograms saving them
// into either the 1D or 2D histogram map based on the number
// of bins provided. If the number of bin arguments isn't correct,
// a warning will be printed and that histogram will be skipped.
void histManager::load_histograms(std::string json_file) {
    std::ifstream histograms(json_file);
    nlohmann::json histo_definitions;
    histograms >> histo_definitions;
    for (auto it = histo_definitions.begin(); it != histo_definitions.end(); it++) {
        if (it.value().size() == 3) {
            hists_1d[it.key()] = new TH1F(it.key().c_str(), it.key().c_str(), it.value().at(0), it.value().at(1), it.value().at(2));
        } else if (it.value().size() == 6) {
            hists_2d[it.key()] = new TH2F(it.key().c_str(), it.key().c_str(), it.value().at(0), it.value().at(1), it.value().at(2), it.value().at(3),
                                          it.value().at(4), it.value().at(5));
        } else {
            std::cout << "Histogram " << it.key() << " has the wrong number of bins. Skipping." << std::endl;
        }
    }
}

// Fill fills the given 1D histogram with the provided variable and event weight.
void histManager::Fill(std::string name, double var, double weight) { hists_1d.at(name)->Fill(var, weight); }

// Fill2d fills the given 2D histogram with the provided variable and event weight.
void histManager::Fill2d(std::string name, double var1, double var2, double weight) { hists_2d.at(name)->Fill(var1, var2, weight); }

// FillBin fills the given 1D histogram at the requested bin with an event weight.
void histManager::FillBin(std::string name, int bin, double weight) {
    auto content = hists_1d.at(name)->GetBinContent(bin);
    hists_1d.at(name)->SetBinContent(bin, content + weight);
}

// FillPrevBins fills the correct bin for the provided var-value as well as
// filling all bins bin_value < var. This is useful for finding things like
// efficiency as a function of pT.
void histManager::FillPrevBins(std::string name, double var, double weight) {
    auto bin = hists_1d.at(name)->FindBin(var);
    for (auto ibin = 0; ibin <= bin; ibin++) {
        auto content = hists_1d.at(name)->GetBinContent(ibin);
        hists_1d.at(name)->SetBinContent(ibin, content + weight);
    }
}

// Write all histograms to the root of the file.
void histManager::Write() {
    fout->cd();
    fout->Write();
    fout->Close();
}

#endif  // INTERFACE_HISTMANAGER_H_
