// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TRIGGER_SF_H_
#define INTERFACE_TRIGGER_SF_H_

#include <string>
#include "TFile.h"
#include "TH2F.h"

// trigger_sf wraps the histogram that stores the trigger
// scale factors and gives helper functions to grab the
// scale factors.
class trigger_sf {
   public:
    explicit trigger_sf(std::string);
    ~trigger_sf() { delete histogram; }
    double get_sf(double, double);

   private:
    TH2F* histogram;
};

// The trigger_sf constructor opens the provided file and stores
// a pointer to the histogram in the file.
trigger_sf::trigger_sf(std::string file_name) {
    auto ifile = reinterpret_cast<TFile*>(new TFile(file_name.c_str(), "READ"));
    histogram = reinterpret_cast<TH2F*>(ifile->Get("trigger_efficiency"));
    ifile->Close();
}

// use mass and pT to grab the scale factor from histogram
double trigger_sf::get_sf(double mass, double pt) { return histogram->GetBinContent(mass, pt); }

#endif  // INTERFACE_TRIGGER_SF_H_
