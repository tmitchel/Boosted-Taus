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

histManager::histManager(std::string fname) : fout(std::make_shared<TFile>(fname.c_str(), "RECREATE")) {
  fout->cd();
  hists_1d = {};  // you can hardcode histograms here
  hists_2d = {};  // you can hardcode histograms here
}

void histManager::load_histograms(std::string json_file) {
  std::ifstream histograms(json_file);
  nlohmann::json histo_definitions;
  histograms >> histo_definitions;
  for (auto it = histo_definitions.begin(); it != histo_definitions.end(); it++) {
    if (it.value().size() == 3) {
      hists_1d[it.key()] = new TH1F(it.key().c_str(), it.key().c_str(), it.value().at(0), it.value().at(1), it.value().at(2));
    } else if (it.value().size() == 6) {
      hists_2d[it.key()] = new TH2F(it.key().c_str(), it.key().c_str(), it.value().at(0), it.value().at(1), it.value().at(2),
                                    it.value().at(3), it.value().at(4), it.value().at(5));
    } else {
      std::cout << "Histogram " << it.key() << " has the wrong number of bins. Skipping." << std::endl;
    }
  }
}

void histManager::Fill(std::string name, double var, double weight) { hists_1d.at(name)->Fill(var, weight); }

void histManager::Fill2d(std::string name, double var1, double var2, double weight) { hists_2d.at(name)->Fill(var1, var2, weight); }

void histManager::FillBin(std::string name, int bin, double weight) {
  auto content = hists_1d.at(name)->GetBinContent(bin);
  hists_1d.at(name)->SetBinContent(bin, content + weight);
}

void histManager::FillPrevBins(std::string name, double var, double weight) {
  auto bin = hists_1d.at(name)->FindBin(var);
  for (auto ibin = 0; ibin <= bin; ibin++) {
    auto content = hists_1d.at(name)->GetBinContent(ibin);
    hists_1d.at(name)->SetBinContent(ibin, content + weight);
  }
}

void histManager::Write() {
  fout->cd();
  fout->Write();
  fout->Close();
}

#endif  // INTERFACE_HISTMANAGER_H_
