// Copyright [2019] Tyler Mitchell

#include <fstream>
#include <iostream>

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

int find_loose_electron(std::shared_ptr<VElectron>);
Electron get_signal_electron(std::shared_ptr<VElectron>);
Electron get_antiid_electron(std::shared_ptr<VElectron>);
Boosted get_signal_tau(std::shared_ptr<VBoosted>);
vector<Electron> get_control_electrons(std::shared_ptr<VElectron>);
vector<Electron> get_antiid_control_electrons(std::shared_ptr<VElectron>);
bool pass_muon_veto(std::shared_ptr<VMuon>);

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
        if (event.getLepTrigger(38)) {  // HLT_Ele115_CaloIdVT_GsfTrkIdT_v
            hists->Fill("cutflow", 1., evtwt);
        } else {
            continue;
        }

        if (pass_muon_veto(muon_factory.getMuons())) {  // found electrons in the event
            hists->Fill("cutflow", 2., evtwt);
        } else {
            continue;
        }

        if (jet_factory.getNBTags() > 0) {  // b-jet veto
            hists->Fill("cutflow", 3., evtwt);
        } else {
            continue;
        }

        auto electrons = electron_factory.getElectrons();
        auto taus = boost_factory.getTaus();

        //////////////////////////////
        // Begin Zmumu CR selection //
        //////////////////////////////

        // get necessary objects
        auto nloose = find_loose_electron(electrons);
        if (nloose <= 2) {
            hists->Fill("cutflow", 4., evtwt);
        } else {
            continue;
        }

        // get passing/failing pairs
        auto good_ele_pair = get_control_electrons(electrons);
        auto anti_ele_pair = get_antiid_control_electrons(electrons);

        // passing region
        if (good_ele_pair.size() == 2) {
            if (good_ele_pair.at(0).getCharge() * good_ele_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_control", (good_ele_pair.at(0).getP4() + good_ele_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_control", (good_ele_pair.at(0).getP4() + good_ele_pair.at(1).getP4()).M(), evtwt);
            }
        } else if (anti_ele_pair.size() == 2) {
            if (anti_ele_pair.at(0).getCharge() * anti_ele_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_anti_control", (anti_ele_pair.at(0).getP4() + anti_ele_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_anti_control", (anti_ele_pair.at(0).getP4() + anti_ele_pair.at(1).getP4()).M(), evtwt);
            }
        }

        ///////////////////////////////////
        // Begin signal region selection //
        ///////////////////////////////////

        if (nloose <= 1) {
            hists->Fill("cutflow", 5., evtwt);
        } else {
            continue;
        }

        auto good_electron = get_signal_electron(electrons);
        auto anti_electron = get_antiid_electron(electrons);
        auto good_tau = get_signal_tau(taus);

        // check if we find a good tau
        if (good_tau.getPt() > 0) {
            hists->Fill("cutflow", 6., evtwt);
        } else {
            continue;
        }

        // check if we found a good passing or failing electron
        if (good_electron.getPt() > 0 || anti_electron.getPt() > 0) {
            hists->Fill("cutflow", 7., evtwt);
        } else {
            continue;
        }

        // get the 4-vectors
        auto el_vector(good_electron.getP4()), anti_vector(anti_electron.getP4()), tau_vector(good_tau.getP4());

        // construct pass-id signal region
        if (el_vector.DeltaR(tau_vector) > 0.4 && el_vector.DeltaR(tau_vector) < 0.8) {
            hists->Fill("cutflow", 8., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 9., evtwt);
                if (good_electron.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("cutflow", 10., evtwt);
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

        // construct anti-id signal region
        if (anti_vector.DeltaR(tau_vector) > 0.4 && anti_vector.DeltaR(tau_vector) < 0.8) {
            if (good_tau.getIso(medium)) {  // tau pass region
                if (anti_electron.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_pass", (anti_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass", (anti_vector + tau_vector).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (anti_electron.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_anti_fail", (anti_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail", (anti_vector + tau_vector).M(), evtwt);
                }
            }
        }
    }      // end event loop
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

int find_loose_electron(std::shared_ptr<VElectron> all_electrons) {
    int loose_ele(0);
    for (auto& ele : *all_electrons) {
        if (ele.getPt() > 10 && fabs(ele.getEta()) < 2.4 && ele.getIDMVANoIso()) {
            loose_ele++;
        }
    }
    return loose_ele;
}

Electron get_signal_electron(std::shared_ptr<VElectron> all_electrons) {
    for (auto ele : *all_electrons) {
        if (ele.getPt() > 55 && fabs(ele.getEta()) < 2.4 && ele.getIDMVANoIso()) {
            return ele;
        }
    }
    return Electron(0, 0, 0, 0);
}

Electron get_antiid_electron(std::shared_ptr<VElectron> all_electrons) {
    for (auto ele : *all_electrons) {
        if (ele.getPt() > 55 && fabs(ele.getEta()) < 2.4 && !ele.getIDMVANoIso()) {
            return ele;
        }
    }
    return Electron(0, 0, 0, 0);
}

Boosted get_signal_tau(std::shared_ptr<VBoosted> all_taus) {
    for (unsigned i = 0; i < all_taus->size(); i++) {
        if (all_taus->at(i).getPt() > 30 && fabs(all_taus->at(i).getEta()) < 2.3
            && all_taus->at(i).getMuRejection(loose) && all_taus->at(i).getEleRejection(tight)) {
            return all_taus->at(i);
        }
    }
    return Boosted(0, 0, 0, 0);
}

vector<Electron> get_control_electrons(std::shared_ptr<VElectron> all_electrons) {
    vector<Electron> good_pair;
    for (auto ele : *all_electrons) {  // tighter electron
        if (ele.getPt() > 55 && fabs(ele.getEta()) < 2.4 && ele.getIDMVANoIso()) {
            for (auto ele2 : *all_electrons) {  // looser electron
                if (ele2.getPt() > 10 && fabs(ele2.getEta()) < 2.4 && ele2.getIDMVANoIso() &&
                    (ele.getP4() + ele2.getP4()).M() > 60 && (ele.getP4() + ele2.getP4()).M() < 120
                    && ele.getP4().DeltaR(ele2.getP4()) > 0.05 && ele.getP4().DeltaR(ele2.getP4()) < 1.5) {
                    good_pair = {ele, ele2};
                }
            }
        }
    }
    return good_pair;
}

vector<Electron> get_antiid_control_electrons(std::shared_ptr<VElectron> all_electrons) {
    vector<Electron> good_pair;
    for (auto ele : *all_electrons) {  // tighter muon
        if (ele.getPt() > 55 && fabs(ele.getEta()) < 2.4 && ele.getIDMVANoIso()) {
            for (auto ele2 : *all_electrons) {  // looser muon
                if (ele2.getPt() > 10 && fabs(ele2.getEta()) < 2.4 && !ele2.getIDMVANoIso() &&
                    (ele.getP4() + ele2.getP4()).M() > 60 && (ele.getP4() + ele2.getP4()).M() < 120
                    && ele.getP4().DeltaR(ele2.getP4()) > 0.05 && ele.getP4().DeltaR(ele2.getP4()) < 1.5) {
                    good_pair = {ele, ele2};
                }
            }
        }
    }
    return good_pair;
}
