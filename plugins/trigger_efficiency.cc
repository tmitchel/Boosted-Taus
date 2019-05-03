// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "../interface/CLParser.h"
#include "../interface/event_factory.h"
#include "../interface/histManager.h"
#include "../interface/jets_factory.h"
#include "../interface/util.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TTree.h"

using std::string;
using std::vector;

int main(int argc, char **argv) {
    auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
    auto verbose = parser->Flag("-v");
    auto is_data = parser->Flag("--data");
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
    auto jet_factory = Jets_Factory(tree, is_data);
    auto event = Event_Factory(tree);
    auto nevt_hist = reinterpret_cast<TH1F *>(fin->Get("hcount"));

    std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
    sample_name = sample_name.substr(0, sample_name.rfind(".root"));
    if (input_name.find("WJetsToLNu_HT-2500ToInf") != std::string::npos) {
        sample_name = "WJetsToLNu_HT-2500ToInf";
    } else if (input_name.find("WJetsToLNu_HT-800To1200") != std::string::npos) {
        sample_name = "WJetsToLNu_HT-800To1200";
    }
    std::cout << sample_name << std::endl;
    double evtwt = lumi["2017"] * cross_sections[sample_name] / nevt_hist->GetBinContent(2);
    if (is_data) {
        evtwt = 1.;
    }

    auto nevts = tree->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (auto i = 0; i < nevts; i++) {
        tree->GetEntry(i);
        if (i == progress * fraction && verbose) {
            std::cout << "\t" << progress * 10 << "% complete. (" << nevts << " total events)\r" << std::flush;
            progress++;
        }

        // run all the factories to fill variables
        jet_factory.Run_Factory();
        event.Run_Factory();
        auto jets = jet_factory.getJets();
        auto MET = event.getMET();

        /////////////////////
        // Event Selection //
        /////////////////////

        // just skim selection for now

        // calculate MHT
        TLorentzVector MHT;
        for (auto jet : *jets) {
            MHT += jet.getP4();
        }

        hists->Fill("cutflow", 1., evtwt);

        // get trigger turn on as function of jet pT
        for (auto pt = 300; pt < 1000; pt += 5) {
            if (jets->at(0).getPt() > pt) {
                hists->Fill("pt_turnon_den", pt, evtwt);
                if ((event.getJetTrigger(39) || event.getJetTrigger(40))) {
                    hists->Fill("pt_turnon", pt, evtwt);
                }
            }
        }

        // get trigger turn on as function of met
        for (auto met = 50; met < 800; met += 5) {
            if (MET.Pt() > met) {
                hists->Fill("met_turnon_den", met, evtwt);
                if ((event.getJetTrigger(39) || event.getJetTrigger(40))) {
                    hists->Fill("met_turnon", met, evtwt);
                }
            }
        }

        // get trigger turn on as function of mht
        for (auto mht = 0; mht < 800; mht++) {
            if (MHT.Pt() > mht) {
                hists->Fill("mht_turnon_den", mht, evtwt);
                if ((event.getJetTrigger(39) || event.getJetTrigger(40))) {
                    hists->Fill("mht_turnon", mht, evtwt);
                }
            }
        }
    }

    if (verbose) {
        std::cout << std::endl;
    }
    fin->Close();
    hists->Write();
    return 1;
}