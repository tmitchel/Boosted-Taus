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
  void FillBin(std::string, int, double);
  void FillPrevBins(std::string, double, double);
  void Write();

 private:
  std::map<std::string, TH1F*> hists;
  std::shared_ptr<TFile> fout;
};

histManager::histManager(std::string fname) : fout(std::make_shared<TFile>(fname.c_str(), "RECREATE")) {
  fout->cd();
  hists = {
      {"pt0_jet_flavor", new TH1F("pt0_jet_flavor", "pt0_jet_flavor", 7, 0.5, 7.5)},
      {"pt400_jet_flavor", new TH1F("pt400_jet_flavor", "pt400_jet_flavor", 7, 0.5, 7.5)},
      {"pt600_jet_flavor", new TH1F("pt600_jet_flavor", "pt600_jet_flavor", 7, 0.5, 7.5)},
      {"pt800_jet_flavor", new TH1F("pt800_jet_flavor", "pt800_jet_flavor", 7, 0.5, 7.5)},
      {"lead_jet_flavor", new TH1F("lead_jet_flavor", "lead_jet_flavor", 7, 0.5, 7.5)},
      {"nevents", new TH1F("nevents", "nevents", 1, 0.5, 1.5)},
      {"dr_taus", new TH1F("dr_taus", "dr_taus", 100, 0, 5)},
      {"dr_jet_MET", new TH1F("dr_jet_MET", "dr_jet_MET", 100, 0, 5)},
      {"dr_higgs_MET", new TH1F("dr_higgs_MET", "dr_higgs_MET", 100, 0, 5)},
      {"dphi_taus", new TH1F("dphi_taus", "dphi_taus", 100, -3.14, 3.14)},
      {"lead_gen_jet_eff", new TH1F("lead_gen_jet_eff", "lead_gen_jet_eff", 100, 0, 1000)},
      {"lead_jet_eff", new TH1F("lead_jet_eff", "lead_jet_eff", 100, 0, 1000)},
      {"cutflow", new TH1F("cutflow", "cutflow", 3, 0.5, 3.5)},

      {"is_prescaled", new TH1F("is_prescaled", "is_prescaled", 8, 0.5, 8.5)},
      {"all_trigger_eff", new TH1F("all_trigger_eff", "all_trigger_eff", 4, 0.5, 4.5)},
      {"lead_jet_pt", new TH1F("lead_jet_pt", "lead_jet_pt", 15, 500, 1000)},
      {"jet320_trigger_eff", new TH1F("jet320_trigger_eff", "jet320_trigger_eff", 15, 500., 1000)},
      {"jet500_trigger_eff", new TH1F("jet500_trigger_eff", "jet500_trigger_eff", 15, 500., 1000)},
      {"HT300_MET110_trigger_eff", new TH1F("HT300_MET110_trigger_eff", "HT300_MET110_trigger_eff", 15, 500., 1000.)},
      {"HT800_trigger_eff", new TH1F("HT800_trigger_eff", "HT800_trigger_eff", 15, 500., 1000.)},

      {"all_iso_eff", new TH1F("all_iso_eff", "all_iso_eff", 3, 0.5, 3.5)},
      {"all_tau_pt", new TH1F("all_tau_pt", "all_tau_pt", 20, 0, 600)},
      {"ntau", new TH1F("ntau", "ntau", 1, 0.5, 1.5)},
      {"loose_boosted_eff", new TH1F("loose_boosted_eff", "loose_boosted_eff", 20, 0., 600.)},
      {"medium_boosted_eff", new TH1F("medium_boosted_eff", "medium_boosted_eff", 20, 0., 600.)},
      {"tight_boosted_eff", new TH1F("tight_boosted_eff", "tight_boosted_eff", 20, 0., 600.)},
  };
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
