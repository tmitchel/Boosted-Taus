// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_FACTORY_H_
#define INTERFACE_JETS_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "TTree.h"

class Jets;
typedef std::vector<Jets> VJets;

class Jets_Factory {
   public:
    Jets_Factory(TTree *, bool);
    void Run_Factory();

    // getters
    Int_t getNTotalJets() { return nJet; }
    Int_t getNGoodJets() { return nGoodJet; }
    Int_t getNBTags() { return nBTag; }
    std::shared_ptr<VJets> getJets() { return std::make_shared<VJets>(jets); }

   private:
    Bool_t is_data;
    Int_t nJet, nGoodJet, nBTag;
    VJets jets, btags;
    std::vector<Bool_t> *jetPFLooseId;
    std::vector<Int_t> *jetPartonID, *jetHadFlvr, *jetID;
    std::vector<Float_t> *jetPt, *jetEta, *jetPhi, *jetEn, *jetCSV2BJetTags, *jetDeepCSVTags_b, *jetDeepCSVTags_bb, *jetDeepCSVTags_c,
        *jetDeepCSVTags_udsg;
};

Jets_Factory::Jets_Factory(TTree *tree, bool is_data_)
    : is_data(is_data_),
      jetPt(nullptr),
      jetEta(nullptr),
      jetPhi(nullptr),
      jetEn(nullptr),
      jetCSV2BJetTags(nullptr),
      jetDeepCSVTags_b(nullptr),
      jetDeepCSVTags_bb(nullptr),
      jetDeepCSVTags_c(nullptr),
      jetDeepCSVTags_udsg(nullptr),
      jetPartonID(nullptr),
      jetHadFlvr(nullptr),
      jetPFLooseId(nullptr),
      jetID(nullptr) {
    tree->SetBranchAddress("nJet", &nJet);
    tree->SetBranchAddress("jetPt", &jetPt);
    tree->SetBranchAddress("jetEn", &jetEn);
    tree->SetBranchAddress("jetEta", &jetEta);
    tree->SetBranchAddress("jetPhi", &jetPhi);
    tree->SetBranchAddress("jetCSV2BJetTags", &jetCSV2BJetTags);
    tree->SetBranchAddress("jetDeepCSVTags_b", &jetDeepCSVTags_b);
    tree->SetBranchAddress("jetDeepCSVTags_bb", &jetDeepCSVTags_bb);
    tree->SetBranchAddress("jetDeepCSVTags_c", &jetDeepCSVTags_c);
    tree->SetBranchAddress("jetDeepCSVTags_udsg", &jetDeepCSVTags_udsg);
    tree->SetBranchAddress("jetPFLooseId", &jetPFLooseId);
    tree->SetBranchAddress("jetID", &jetID);
    if (!is_data) {
        tree->SetBranchAddress("jetPartonID", &jetPartonID);
        tree->SetBranchAddress("jetHadFlvr", &jetHadFlvr);
    }
}

void Jets_Factory::Run_Factory() {
    jets.clear();
    btags.clear();
    Jets jet;
    for (auto i = 0; i < nJet; i++) {
        if (jetPt->at(i) < 30 || fabs(jetEta->at(i)) > 3) {
            continue;
        }
        jet = Jets(jetPt->at(i), jetEta->at(i), jetPhi->at(i), jetEn->at(i));
        jet.CSV2BJetTags = jetCSV2BJetTags->at(i);
        jet.DeepCSVTags_b = jetDeepCSVTags_b->at(i);
        jet.DeepCSVTags_bb = jetDeepCSVTags_bb->at(i);
        jet.DeepCSVTags_c = jetDeepCSVTags_c->at(i);
        jet.DeepCSVTags_udsg = jetDeepCSVTags_udsg->at(i);
        jet.PFLooseId = jetPFLooseId->at(i);
        jet.ID = jetID->at(i);
        if (!is_data) {
            jet.PartonID = jetPartonID->at(i);
            jet.HadFlvr = jetHadFlvr->at(i);
        }
        jets.push_back(jet);

        if (jetCSV2BJetTags->at(i) > 0.4941) {
            btags.push_back(jet);
        }
    }

    // sort by pT
    std::sort(jets.begin(), jets.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
    std::sort(btags.begin(), btags.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodJet = jets.size();
    nBTag = btags.size();
}

class Jets {
    friend class Jets_Factory;

   public:
    Jets(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Jets() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Bool_t getJetPFLooseId() { return PFLooseId; }
    Int_t getPartonID() { return PartonID; }
    Int_t getHadFlvr() { return HadFlvr; }
    Int_t getID() { return ID; }
    Float_t getCSV2BJetTags() { return CSV2BJetTags; }
    Float_t getDeepCSVTags_b() { return DeepCSVTags_b; }
    Float_t getDeepCSVTags_bb() { return DeepCSVTags_bb; }
    Float_t getDeepCSVTags_c() { return DeepCSVTags_c; }
    Float_t getDeepCSVTags_udsg() { return DeepCSVTags_udsg; }

   private:
    TLorentzVector p4;
    Bool_t PFLooseId;
    Int_t PartonID, HadFlvr, ID;
    Float_t CSV2BJetTags, DeepCSVTags_b, DeepCSVTags_bb, DeepCSVTags_c, DeepCSVTags_udsg;
};

#endif  // INTERFACE_JETS_FACTORY_H_
