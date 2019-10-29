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

bool pass_muon_veto(std::shared_ptr<VMuon>);
VJets analysis_jets(std::shared_ptr<VJets>);
VElectron analysis_electrons(std::shared_ptr<VElectron>);
VBoosted analysis_taus(std::shared_ptr<VBoosted>);
bool calculate_electron_iso(Electron);

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
        if (event.getLepTrigger(3) || event.getLepTrigger(4)) {  // HLT_Ele35_WPTight_Gsf_v || HLT_Ele27_WPTight_Gsf_v
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

        if (pass_muon_veto(muon_factory.getMuons())) {  // no muons in the event
            hists->Fill("cutflow", 4., evtwt);
        } else {
            continue;
        }

        // get leptons
        auto electrons = analysis_electrons(electron_factory.getElectrons());
        auto taus = analysis_taus(boost_factory.getTaus());

        ///////////////////////////////////
        // Begin signal region selection //
        ///////////////////////////////////

        // veto on too many electrons
        if (electrons.size() < 2) {
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

        // check if we found a good passing or failing electrons
        if (electrons.size() > 0) {
            hists->Fill("cutflow", 8., evtwt);
        } else {
            continue;
        }

        bool good_match(false);
        Electron good_electron;
        Boosted good_tau;
        for (auto& tau : taus) {
            for (auto& el : electrons) {
                if (el.getP4().DeltaR(tau.getP4()) > 0.4 && el.getP4().DeltaR(tau.getP4()) < 0.8) {
                    good_match = true;
                    good_electron = el;
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

        auto pass_electron_isolation = calculate_electron_iso(good_electron);
        auto el_vector(good_electron.getP4());
        auto tau_vector(good_tau.getP4());

        // construct pass-iso signal region
        if (pass_electron_isolation) {
            hists->Fill("cutflow", 10., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 11., evtwt);
                if (good_electron.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_pass", (el_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_pass", (el_vector + tau_vector).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (good_electron.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_fail", (el_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_fail", (el_vector + tau_vector).M(), evtwt);
                }
            }
        }

        // construct anti-iso signal region
        if (!pass_electron_isolation) {
            hists->Fill("cutflow", 10., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 11., evtwt);
                if (good_electron.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_pass", (el_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass", (el_vector + tau_vector).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (good_electron.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_anti_fail", (el_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail", (el_vector + tau_vector).M(), evtwt);
                }
            }
        }
    }  // end event loop
    fin->Close();
    hists->Write();
    logfile.close();
}

bool pass_muon_veto(std::shared_ptr<VMuon> all_muons) {
    for (auto mu : *all_muons) {
        if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.5) {
            return false;  // veto any muons in the event
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

VElectron analysis_electrons(std::shared_ptr<VElectron> all_electrons) {
    VElectron good_electrons;
    for (auto& el : *all_electrons) {
        if (el.getPt() > 40 && fabs(el.getEta()) < 2.5) {
            good_electrons.push_back(el);
        }
    }
    return good_electrons;
}

VBoosted analysis_taus(std::shared_ptr<VBoosted> all_taus) {
    VBoosted good_taus;
    for (unsigned i = 0; i < all_taus->size(); i++) {
        if (all_taus->at(i).getPt() > 20 && fabs(all_taus->at(i).getEta()) < 2.3 && all_taus->at(i).getMuRejection(loose) &&
            all_taus->at(i).getEleRejection(tight) && all_taus->at(i).getDiscByDM(false) > 0.5) {
            good_taus.push_back(all_taus->at(i));
        }
    }
    return good_taus;
}

bool calculate_electron_iso(Electron el) {
    if (fabs(el.getSCEta()) < 0.8 && el.getIDMVAIso() > 0.941) {
        return true;
    } else if (fabs(el.getSCEta()) > 0.8 && fabs(el.getSCEta()) < 1.5 && el.getIDMVAIso() > 0.899) {
        return true;
    } else if (fabs(el.getSCEta()) > 1.5 && el.getIDMVAIso() > 0.758) {
        return true;
    }
    return false;
}