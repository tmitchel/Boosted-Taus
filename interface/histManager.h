// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_HISTMANAGER_H_
#define INTERFACE_HISTMANAGER_H_

#include <map>
#include <memory>
#include <string>
#include "TFile.h"
#include "TH1F.h"

class histManager {
 public:
  explicit histManager(std::string);
  ~histManager() {}
  void Fill(std::string, double, double);
  void Write();

 private:
  std::map<std::string, TH1F*> hists;
  std::shared_ptr<TFile> fout;
};

histManager::histManager(std::string fname) : fout(std::make_shared<TFile>(fname.c_str(), "RECREATE")) {
  fout->cd();
  hists = {
    {"lowpt_taus", new TH1F("lowpt_taus", "lowpt_taus", 50, 0, 250)},
    {"highpt_taus", new TH1F("highpt_taus", "highpt_taus", 100, 0, 500)},
    {"lowpt_boost", new TH1F("lowpt_boost", "lowpt_boost", 50, 0, 350)},
    {"highpt_boost", new TH1F("highpt_boost", "highpt_boost", 100, 0, 500)},
    {"allpt_dr_reco", new TH1F("allpt_dr_reco", "allpt_dr", 100, 0, 5)},
    {"lowpt_dr_reco", new TH1F("lowpt_dr_reco", "lowpt_dr", 100, 0, 5)},
    {"highpt_dr_reco", new TH1F("highpt_dr_reco", "highpt_dr", 100, 0, 5)},
    {"allpt_dr_gen", new TH1F("allpt_dr_gen", "allpt_dr", 100, 0, 5)},
    {"lowpt_dr_gen", new TH1F("lowpt_dr_gen", "lowpt_dr", 100, 0, 5)},
    {"highpt_dr_gen", new TH1F("highpt_dr_gen", "highpt_dr", 100, 0, 5)}};
}

void histManager::Fill(std::string name, double var, double weight) { hists.at(name)->Fill(var, weight); }

void histManager::Write() {
  fout->cd();
  // for (auto ihist = hists.begin(); ihist != hists.end(); ihist++) {
  //   ihist->second->Write();
  // }
  fout->Write();
  fout->Close();
}

#endif  // INTERFACE_HISTMANAGER_H_
