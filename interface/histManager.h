// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_HISTMANAGER_H_
#define INTERFACE_HISTMANAGER_H_

#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include "./json.hpp"
#include "TFile.h"
#include "TH1F.h"

class histManager {
 public:
  explicit histManager(std::string);
  ~histManager() {}
  void load_histograms(std::string);
  void Fill(std::string, double, double);
  void FillBin(std::string, int, double);
  void FillPrevBins(std::string, double, double);
  void Write();

 private:
  std::map<std::string, TH1F*> hists;
  std::shared_ptr<TFile> fout;
};

histManager::histManager(std::string fname) : fout(std::make_shared<TFile>(fname.c_str(), "RECREATE")) {
  fout->cd();
  hists = {};  // you can hardcode histograms here
}

void histManager::load_histograms(std::string json_file) {
  std::ifstream histograms(json_file);
  nlohmann::json histo_definitions;
  histograms >> histo_definitions;
  for (auto it = histo_definitions.begin(); it != histo_definitions.end(); it++) {
    if (it.value().size() == 3) {
      hists[it.key()] = new TH1F(it.key().c_str(), it.key().c_str(), it.value().at(0), it.value().at(1), it.value().at(2));
    } else {
      std::cout << "Histogram " << it.key() << " has the wrong number of bins. Skipping." << std::endl;
    }
  }
}

void histManager::Fill(std::string name, double var, double weight) { hists.at(name)->Fill(var, weight); }

void histManager::FillBin(std::string name, int bin, double weight) {
  auto content = hists.at(name)->GetBinContent(bin);
  hists.at(name)->SetBinContent(bin, content + weight);
}

void histManager::FillPrevBins(std::string name, double var, double weight) {
  auto bin = hists.at(name)->FindBin(var);
  for (auto ibin = 0; ibin <= bin; ibin++) {
    auto content = hists.at(name)->GetBinContent(ibin);
    hists.at(name)->SetBinContent(ibin, content + weight);
  }
}

void histManager::Write() {
  fout->cd();
  fout->Write();
  fout->Close();
}

#endif  // INTERFACE_HISTMANAGER_H_
