// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_FACTORY_H_
#define INTERFACE_JETS_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

class Jets;  // Jets combines all tau information
typedef std::vector<Jets> VJets;

// Jets combines all jet related information along with
// creating the jet 4-vector. Helper functions are provided
// to access properties of the jets. These properties cannot
// be updated.
class Jets {
    friend class Jets_Factory;  // Jet_Factory constructs individual Jets

   public:
    Jets(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Jets() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() const { return p4.Pt(); }
    Float_t getEta() const { return p4.Eta(); }
    Float_t getPhi() const { return p4.Phi(); }
    Float_t getMass() const { return p4.M(); }
    Bool_t getJetPFLooseId() const { return PFLooseId; }
    Int_t getPartonID() const { return PartonID; }
    Int_t getHadFlvr() const { return HadFlvr; }
    Int_t getID() const { return ID; }
    Float_t getCSV2BJetTags() const { return CSV2BJetTags; }
    Float_t getDeepCSVTags_b() const { return DeepCSVTags_b; }
    Float_t getDeepCSVTags_bb() const { return DeepCSVTags_bb; }
    Float_t getDeepCSVTags_c() const { return DeepCSVTags_c; }
    Float_t getDeepCSVTags_udsg() const { return DeepCSVTags_udsg; }

   private:
    // these should never be modified once read from the TTree
    TLorentzVector p4;
    Bool_t PFLooseId;
    Int_t PartonID, HadFlvr, ID;
    Float_t CSV2BJetTags, DeepCSVTags_b, DeepCSVTags_bb, DeepCSVTags_c, DeepCSVTags_udsg;
};

// Jets_Factory reads from the TTree and constructs
// individual Jets objects. The Jets_Factory holds the
// list of jets, but can provided a shared_ptr to the
// list.
class Jets_Factory {
   public:
    Jets_Factory(TTree *, bool);
    void Run_Factory();

    // getters
    Int_t getNTotalJets() { return nJet; }     // nJets directly from TTree
    Int_t getNGoodJets() { return nGoodJet; }  // nJets passing preselection
    Int_t getNBTags() { return nBTag; }        // nBtags passing preselection
    std::shared_ptr<VJets> getJets() { return std::make_shared<VJets>(jets); }
    Double_t HT(const VJets &);

   private:
    Bool_t is_data;
    Int_t nJet, nGoodJet, nBTag;
    VJets jets, btags;
    std::vector<Bool_t> *jetPFLooseId;
    std::vector<Int_t> *jetPartonID, *jetHadFlvr, *jetID;
    std::vector<Float_t> *jetPt, *jetEta, *jetPhi, *jetEn, *jetCSV2BJetTags, *jetDeepCSVTags_b, *jetDeepCSVTags_bb, *jetDeepCSVTags_c,
        *jetDeepCSVTags_udsg;
};

// Set all branch addresses when constructing a Jets_Factory.
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
    if (!is_data) {  // not available in data
        tree->SetBranchAddress("jetPartonID", &jetPartonID);
        tree->SetBranchAddress("jetHadFlvr", &jetHadFlvr);
    }
}

// Called once per event to construct the Jets. A basic
// preselection is applied to all jets. All Jets passing
// preselection are sorted by pT and stored.
void Jets_Factory::Run_Factory() {
    jets.clear();
    btags.clear();
    Jets jet;
    for (auto i = 0; i < nJet; i++) {
        // baseline/default selection
        if (jetPt->at(i) < 20 || fabs(jetEta->at(i)) > 3 || jetPFLooseId->at(i) < 0.5) {
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

        // medium b-jets
        if (jetCSV2BJetTags->at(i) > 0.8838 && fabs(jetEta->at(i)) < 2.4) {
            btags.push_back(jet);
        }
    }

    // sort by pT
    std::sort(jets.begin(), jets.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
    std::sort(btags.begin(), btags.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodJet = jets.size();
    nBTag = btags.size();
}

// HT calculates the scalar sum of all hadronic
// activity in the event.
Double_t Jets_Factory::HT(const VJets &jets) {
    double ht(0.);
    for (auto &jet : jets) {
        ht += jet.getPt();
    }
    return ht;
}

#endif  // INTERFACE_JETS_FACTORY_H_
