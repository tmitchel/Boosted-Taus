// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TRIGGER_SF_H_
#define INTERFACE_TRIGGER_SF_H_

#include <string>
#include "TFile.h"
#include "TH2F.h"

class trigger_sf {
 public:
    explicit trigger_sf(std::string);
    ~trigger_sf() { delete histogram; }
    double get_sf(double, double);
 private:
    TH2F* histogram;
};

trigger_sf::trigger_sf(std::string file_name) {
    auto ifile = reinterpret_cast<TFile*>(new TFile(file_name.c_str(), "READ"));
    histogram = reinterpret_cast<TH2F*>(ifile->Get("trigger_efficiency"));
    ifile->Close();
}

double trigger_sf::get_sf(double mass, double pt) {
    return histogram->GetBinContent(mass, pt);
}

#endif  // INTERFACE_TRIGGER_SF_H_
