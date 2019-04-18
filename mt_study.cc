// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/CLParser.h"
#include "interface/boosted_factory.h"
#include "interface/event_factory.h"
#include "interface/gen_factory.h"
#include "interface/histManager.h"
#include "interface/jets_factory.h"
#include "interface/muon_factory.h"

using std::string;
using std::vector;

int main(int argc, char **argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto verbose = parser->Flag("-v");
  auto input_name = parser->Option("-i");
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
  auto histograms = parser->Option("-j", "test.json");

  // read the input TFile/TTree
  auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
  auto hists = std::make_shared<histManager>(output_name);
  hists->load_histograms(histograms);
  auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
  // construct our object factories
  auto gen_factory = Gen_Factory(tree);
  auto boost_factory = Boosted_Factory(tree);
  auto jet_factory = Jets_Factory(tree);
  auto muon_factory = Muon_Factory(tree);
  auto event = Event_Factory(tree);

  auto nevts = tree->GetEntries();
  int progress(1), fraction((nevts - 1) / 10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress * 10 << "% complete.\r" << std::flush;
      progress++;
    }

    // run all the factories to fill variables
    gen_factory.Run_Factory();
    boost_factory.Run_Factory();
    jet_factory.Run_Factory();
    muon_factory.Run_Factory();
    auto gens = gen_factory.getGens();
    auto boosts = boost_factory.getTaus();
    auto jets = jet_factory.getJets();
    auto muons = muon_factory.getMuons();

    hists->FillBin("all", 1., 1.);

    // only look at mutau channel
    bool isMuTau(false);
    for (auto& gen : *gens) {
      if (fabs(gen.getPID()) == 13 && fabs(gen.getMomPID()) == 15) {
        isMuTau = true;
        break;
      }
    }

    hists->FillBin("some", 1., 1.);

    // now make the cut
    if (!isMuTau) {
      continue;
    }

    // get our Z boson
    double best_mass(999);
    TLorentzVector z_boson, lead_muon;
    for (auto& mu1 : *muons) {
      for (auto& mu2 : *muons) {
        auto muon1 = mu1.getP4();
        auto muon2 = mu2.getP4();
        if (fabs(91.2 - (muon1 + muon2).M()) < fabs(91.2 - best_mass)) {
          best_mass = (muon1 + muon2).M();
          lead_muon = muon1.Pt() > muon2.Pt() ? muon1 : muon2;
          z_boson = (muon1 + muon2);
        }
      }
    }

    auto lead_jet = jets->at(0);
    if (lead_jet.getP4().DeltaR(z_boson) < 2.5) {
      continue;
    }

    auto mu_pt(-999.), best_dr(999.);
    for (auto& boost : *boosts) {
      if (lead_muon.DeltaR(boost.getP4()) < best_dr) {
          best_dr = lead_muon.DeltaR(boost.getP4());
          mu_pt = lead_muon.Pt();
      }
    }

    hists->Fill("closest_mu_pt", mu_pt, 1.);
  }

  if (verbose) {
    std::cout << std::endl;
  }
  fin->Close();
  hists->Write();
  return 1;
}
