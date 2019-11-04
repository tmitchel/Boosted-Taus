// Copyright [2019] Tyler Mitchell

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include "TTree.h"

// utilities
#include "../interface/CLParser.h"
#include "../interface/histManager.h"

// Objects
#include "../interface/electron_factory.h"
#include "../interface/event_factory.h"
#include "../interface/jets_factory.h"
#include "../interface/muon_factory.h"
#include "../interface/util.h"

using std::string;
using std::vector;

bool pass_electron_veto(std::shared_ptr<VElectron>);
VJets analysis_jets(std::shared_ptr<VJets>);
VMuon analysis_muons(std::shared_ptr<VMuon>);
bool calculate_muon_iso(Muon);

int main(int argc, char** argv) {
    auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
    auto verbose = parser->Flag("-v");
    auto is_data = parser->Flag("--data");
    auto input_name = parser->Option("-i");
    auto output_name = parser->Option("-o");
    auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
    auto histograms = parser->Option("-j", "test.json");

    std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
    sample_name = sample_name.substr(0, sample_name.rfind(".root"));

    // create the log file
    std::ofstream logfile;
    std::string logname = output_name.substr(0, sample_name.rfind(".root")) + ".log";
    logfile.open(logname, std::ios::out | std::ios::trunc);
    logfile << "Processing file: " << input_name << std::endl;
    logfile << "Using options:" << std::endl;
    logfile << "\t is_data:     " << is_data << std::endl;
    logfile << "\t output_name: " << output_name << std::endl;
    logfile << "\t tree_name:   " << tree_name << std::endl;
    logfile << "\t histograms:  " << histograms << std::endl;

    // read the input TFile/TTree
    auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
    auto hists = std::make_shared<histManager>(output_name);
    hists->load_histograms(histograms);
    auto tree = reinterpret_cast<TTree*>(fin->Get(tree_name.c_str()));

    // construct our object factories
    auto jet_factory = Jets_Factory(tree, is_data);
    auto muon_factory = Muon_Factory(tree);
    auto electron_factory = Electron_Factory(tree);
    auto event = Event_Factory(tree);
    auto nevt_hist = reinterpret_cast<TH1F*>(fin->Get("hcount"));
    auto cross_section = cross_sections[sample_name];
    Double_t init_weight(cross_section * lumi["2017"] / nevt_hist->GetBinContent(2));
    if (is_data) {
        init_weight = 1.;
    }

    auto nevts = tree->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (auto i = 0; i < nevts; i++) {
        tree->GetEntry(i);
        if (i == progress * fraction && verbose) {
            logfile << "\t" << progress * 10 << "% complete. (" << nevts << " total events)" << std::endl;
            progress++;
        }

        auto evtwt = init_weight;
        jet_factory.Run_Factory();
        muon_factory.Run_Factory();
        electron_factory.Run_Factory();
        event.Run_Factory();

        /////////////////////////
        // Begin pre-selection //
        /////////////////////////
        if (event.getLepTrigger(19)) {  // HLT_IsoMu24_v
            hists->Fill("cutflow", 1., evtwt);
        } else {
            continue;
        }

        if (jet_factory.getNBTags() > 0) {  // b-jet veto
            hists->Fill("cutflow", 2., evtwt);
        } else {
            continue;
        }

        // calculate get good jets and HT
        auto jets = analysis_jets(jet_factory.getJets());
        auto HT = jet_factory.HT(jets);
        if (HT > 200) {
            hists->Fill("cutflow", 3., evtwt);
        } else {
            continue;
        }

        if (pass_electron_veto(electron_factory.getElectrons())) {  // no electrons in the event
            hists->Fill("cutflow", 4., evtwt);
        } else {
            continue;
        }

        // get leptons
        auto muons = analysis_muons(muon_factory.getMuons());

        //////////////////////////////
        // Begin Zmumu CR selection //
        //////////////////////////////

        // only dimuon events
        if (muons.size() == 2) {
            hists->Fill("cutflow", 5., evtwt);
        } else {
            continue;
        }

        // lead muon needs to be harder
        if (muons.at(0).getPt() > 30) {
            hists->Fill("cutflow", 6., evtwt);
        } else {
            continue;
        }

        // get our dimuon pair
        auto lead_muon = muons.at(0);
        auto sub_muon = muons.at(1);

        // make sure they are near each other (somewhat boosted)
        if (lead_muon.getP4().DeltaR(sub_muon.getP4()) < 1.) {
            hists->Fill("cutflow", 7., evtwt);
        } else {
            continue;
        }

        // reconstruct Z and make sure it has sufficient pT
        auto recoZ = (lead_muon.getP4() + sub_muon.getP4());
        if (recoZ.Pt() > 40) {
            hists->Fill("cutflow", 8., evtwt);
        }  else {
            continue;
        }

        // fill control histograms
        if (lead_muon.getCharge() * sub_muon.getCharge() < 0) {
            hists->Fill("OS_pass", recoZ.M(), evtwt);
        } else {
            hists->Fill("SS_pass", recoZ.M(), evtwt);
        }
    }  // end event loop
    fin->Close();
    hists->Write();
    logfile.close();
}

bool pass_electron_veto(std::shared_ptr<VElectron> all_electrons) {
    for (auto el : *all_electrons) {
        if (el.getPt() > 10 && fabs(el.getEta()) < 2.4) {
            return false;  // veto any electrons in the event
        }
    }
    return true;
}

VJets analysis_jets(std::shared_ptr<VJets> jets) {
    VJets good_jets;
    for (auto& jet : *jets) {
        // jets are sorted by pT so once we find one with pT < 30
        // there's no point in looping through the rest
        if (jet.getPt() < 30) {
            break;
        }

        // apply eta selection
        if (fabs(jet.getEta()) < 2.4) {
            good_jets.push_back(jet);
        }
    }
    return good_jets;
}


VMuon analysis_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_muons;
    for (auto& mu : *all_muons) {
        if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.4 && mu.getID(medium)
            && fabs(mu.getD0()) < 0.045 && fabs(mu.getDz()) < 0.2) {
            if (calculate_muon_iso(mu) < 0.2) {
                good_muons.push_back(mu);
            }
        }
    }
    return good_muons;
}

bool calculate_muon_iso(Muon mu) {
    auto iso = mu.getPFChIso() / mu.getPt();
    iso += std::max(0., mu.getPFNeuIso() + mu.getPFPhoIso() - 0.5 * mu.getPFPUIso()) / mu.getPt();
    return iso < 0.2;
}