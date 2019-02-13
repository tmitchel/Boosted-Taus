// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_HISTMANAGER_H_
#define INTERFACE_HISTMANAGER_H_

#include "TH1F.h"
#include <map>
#include <string>

class histManager {
 public:
  histManager();
  ~histManager() {}
  void Fill(std::string, double, double);

 private:
  std::map<std::string, TH1F*> hists;
};

histManager::histManager() : hists{
                                {"lowpt_taus", new TH1F("lowpt_taus", "lowpt_taus", 50, 0, 250)},
                                {"highpt_taus", new TH1F("highpt_taus", "highpt_taus", 100, 0, 500)},
                                {"lowpt_boost", new TH1F("lowpt_boost", "lowpt_boost", 50, 0, 350)},
                                {"highpt_boost", new TH1F("highpt_boost", "highpt_boost", 100, 0, 500)},
                                {"lowpt_dr", new TH1F("lowpt_dr", "lowpt_dr", 100, 0, 5)},
                                {"highpt_dr", new TH1F("highpt_dr", "highpt_dr", 100, 0, 5)},
                                {"lowpt_tau_eff_den", new TH1F("lowpt_tau_eff_den", "lowpt_tau_eff_den", 50, 0, 1000)},
                                {"lowpt_tau_eff_num", new TH1F("lowpt_tau_eff_num", "lowpt_tau_eff_num", 50, 0, 1000)},
                                {"lowpt_boost_eff_den", new TH1F("lowpt_boost_eff_den", "lowpt_boost_eff_den", 50, 0, 1000)},
                                {"lowpt_boost_eff_num", new TH1F("lowpt_boost_eff_num", "lowpt_boost_eff_num", 50, 0, 1000)},
                                {"highpt_tau_eff_den", new TH1F("highpt_tau_eff_den", "highpt_tau_eff_den", 50, 0, 1000)},
                                {"highpt_tau_eff_num", new TH1F("highpt_tau_eff_num", "highpt_tau_eff_num", 50, 0, 1000)},
                                {"highpt_boost_eff_den", new TH1F("highpt_boost_eff_den", "highpt_boost_eff_den", 50, 0, 1000)},
                                {"highpt_boost_eff_num", new TH1F("highpt_boost_eff_num", "highpt_boost_eff_num", 50, 0, 1000)}} {}

void histManager::Fill(std::string name, double var, double weight) {
    hists.at(name)->Fill(var, weight);
}

#endif  // INTERFACE_HISTMANAGER_H_
