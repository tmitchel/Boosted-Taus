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
#include "../interface/boosted_factory.h"
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
VBoosted analysis_taus(std::shared_ptr<VBoosted>);
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
    auto boost_factory = Boosted_Factory(tree);
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
        boost_factory.Run_Factory();
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

        if (jet_factory.getNBTags() == 0) {  // b-jet veto
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
        auto taus = analysis_taus(boost_factory.getTaus());

        ///////////////////////////////////
        // Begin signal region selection //
        ///////////////////////////////////

        // veto on too many muons
        if (muons.size() < 2) {
            hists->Fill("cutflow", 5., evtwt);
        } else {
            continue;
        }

        // check if we find a good tau
        if (taus.size() > 0) {
            hists->Fill("cutflow", 7., evtwt);
        } else {
            continue;
        }

        // check if we found a good passing or failing muon
        if (muons.size() > 0) {
            hists->Fill("cutflow", 8., evtwt);
        } else {
            continue;
        }

        bool good_match(false);
        Muon good_muon;
        Boosted good_tau;
        for (auto& tau : taus) {
            for (auto& mu : muons) {
                if (mu.getP4().DeltaR(tau.getP4()) > 0.4 && mu.getP4().DeltaR(tau.getP4()) < 0.8) {
                    good_match = true;
                    good_muon = mu;
                    good_tau = tau;
                    break;
                }
            }
        }

        // found a matched mu/tau pair
        if (good_match) {
            hists->Fill("cutflow", 9., evtwt);
        } else {
            continue;
        }

        auto pass_muon_isolation = calculate_muon_iso(good_muon);
        auto mu_vector(good_muon.getP4());
        auto tau_vector(good_tau.getP4());

        // construct pass-iso signal region
        if (pass_muon_isolation) {
            hists->Fill("cutflow", 10., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 11., evtwt);
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_pass/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("OS_pass/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                } else {
                    hists->Fill("SS_pass/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("SS_pass/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_fail/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("OS_fail/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                } else {
                    hists->Fill("SS_fail/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("SS_fail/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                }
            }
        }

        // construct anti-iso signal region
        if (!pass_muon_isolation) {
            hists->Fill("cutflow", 10., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 11., evtwt);
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_pass/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("OS_anti_pass/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("SS_anti_pass/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_anti_fail/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("OS_anti_fail/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail/Z_mass", (mu_vector + tau_vector).M(), evtwt);
                    hists->Fill("SS_anti_fail/Z_pt", (mu_vector + tau_vector).Pt(), evtwt);
                }
            }
        }
    }  // end event loop
    fin->Close();
    hists->Write();
    logfile.close();
}

// pass_electron_veto returns false if the event has any
// loose electrons. This is used to veto extra electrons in the
// event.
bool pass_electron_veto(std::shared_ptr<VElectron> all_electrons) {
    for (auto el : *all_electrons) {
        if (el.getPt() > 10 && fabs(el.getEta()) < 2.4) {
            return false;  // veto any electrons in the event
        }
    }
    return true;
}

// analysis_jets returns all jets passing selection for this
// analysis.
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

// analysis_muons returns all muons passing the muon
// selection.
VMuon analysis_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_muons;
    for (auto& mu : *all_muons) {
        if (mu.getPt() > 60 && fabs(mu.getEta()) < 2.4 && mu.getID(medium)
            && fabs(mu.getD0()) < 0.045 && fabs(mu.getDz()) < 0.2) {
            good_muons.push_back(mu);
        }
    }
    return good_muons;
}

// analysis_taus returns all taus passing the tau
// selection.
VBoosted analysis_taus(std::shared_ptr<VBoosted> all_taus) {
    VBoosted good_taus;
    for (unsigned i = 0; i < all_taus->size(); i++) {
        if (all_taus->at(i).getPt() > 20 && fabs(all_taus->at(i).getEta()) < 2.3 && all_taus->at(i).getMuRejection(tight) &&
            all_taus->at(i).getEleRejection(vloose) && all_taus->at(i).getDiscByDM(false) > 0.5) {
            good_taus.push_back(all_taus->at(i));
        }
    }
    return good_taus;
}

// calculate_muon_iso returns true if the muon
// passes the isolation requirements and false otherwise.
bool calculate_muon_iso(Muon mu) {
    auto iso = mu.getPFChIso() / mu.getPt();
    iso += std::max(0., mu.getPFNeuIso() + mu.getPFPhoIso() - 0.5 * mu.getPFPUIso()) / mu.getPt();
    return iso < 0.2;
}
