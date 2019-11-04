// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_AK8_FACTORY_H_
#define INTERFACE_AK8_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

class AK8;  // AK8 combines all tau information
typedef std::vector<AK8> VAK8;

// AK8 combines all tau related information along with
// creating the ak8 jet 4-vector. Helper functions are provided
// to access properties of the ak8 jets. These properties cannot
// be updated.
class AK8 {
    friend class AK8_Factory;  // AK8_Factory constructs individual AK8s

   public:
    AK8(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    AK8() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Bool_t getJetPFLooseId() { return jetPFLooseId; }
    Int_t getPartonID() { return jetPartonID; }
    Int_t getHadFlvr() { return jetHadFlvr; }
    Float_t getPrunedMass() { return AK8JetPrunedMass; }
    Float_t getSoftDropMass() { return AK8JetSoftDropMass; }

   private:
    // these should never be modified once read from the TTree
    TLorentzVector p4;
    Bool_t jetPFLooseId;
    Int_t jetPartonID, jetHadFlvr;
    ULong64_t jetFiredTrgs;
    Float_t AK8JetPrunedMass, AK8JetSoftDropMass;
};

// AK8_Factory reads from the TTree and constructs
// individual AK8 objects. The AK8_Factory holds the
// list of ak8 jets, but can provided a shared_ptr to the
// list.
class AK8_Factory {
   public:
    AK8_Factory(TTree *, bool);
    void Run_Factory();

    // getters
    Int_t getNTotalAK8() { return nJet; }     // nJet directly from TTree
    Int_t getNGoodAK8() { return nGoodJet; }  // nJet passing preselection
    std::shared_ptr<VAK8> getAK8() { return std::make_shared<VAK8>(jets); }

   private:
    Bool_t is_data;
    Int_t nJet, nGoodJet;
    VAK8 jets;
    std::vector<Bool_t> *jetPFLooseId;
    std::vector<Int_t> *jetPartonID, *jetHadFlvr;
    std::vector<Float_t> *jetPt, *jetEta, *jetPhi, *jetEn, *AK8JetPrunedMass, *AK8JetSoftDropMass;
};

// Set all branch addresses when constructing a AK8_Factory.
AK8_Factory::AK8_Factory(TTree *tree, bool is_data_)
    : is_data(is_data_),
      jetPt(nullptr),
      jetEta(nullptr),
      jetPhi(nullptr),
      jetEn(nullptr),
      jetPartonID(nullptr),
      jetHadFlvr(nullptr),
      jetPFLooseId(nullptr),
      AK8JetPrunedMass(nullptr),
      AK8JetSoftDropMass(nullptr) {
    tree->SetBranchAddress("nAK8Jet", &nJet);
    tree->SetBranchAddress("AK8JetPt", &jetPt);
    tree->SetBranchAddress("AK8JetEn", &jetEn);
    tree->SetBranchAddress("AK8JetEta", &jetEta);
    tree->SetBranchAddress("AK8JetPhi", &jetPhi);
    tree->SetBranchAddress("AK8JetPFLooseId", &jetPFLooseId);
    tree->SetBranchAddress("AK8JetPrunedMass", &AK8JetPrunedMass);
    tree->SetBranchAddress("AK8JetSoftDropMass", &AK8JetSoftDropMass);
    if (!is_data) {  // not available in data
        tree->SetBranchAddress("AK8JetPartonID", &jetPartonID);
        tree->SetBranchAddress("AK8JetHadFlvr", &jetHadFlvr);
    }
}

// Called once per event to construct the AK8s. A basic
// preselection is applied to all ak8 jets. All AK8s passing
// preselection are sorted by pT and stored.
void AK8_Factory::Run_Factory() {
    jets.clear();
    for (auto i = 0; i < nJet; i++) {
        if (jetPt->at(i) < 30 || fabs(jetEta->at(i)) > 3) {  // baseline cuts
            continue;
        }
        auto jet = AK8(jetPt->at(i), jetEta->at(i), jetPhi->at(i), jetEn->at(i));
        jet.jetPFLooseId = jetPFLooseId->at(i);
        jet.AK8JetPrunedMass = AK8JetPrunedMass->at(i);
        jet.AK8JetSoftDropMass = AK8JetSoftDropMass->at(i);
        if (!is_data) {  // not available in data
            jet.jetPartonID = jetPartonID->at(i);
            jet.jetHadFlvr = jetHadFlvr->at(i);
        }
        jets.push_back(jet);
    }

    // sort by pT
    std::sort(jets.begin(), jets.end(), [](AK8 &p1, AK8 &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodJet = jets.size();
}

#endif  // INTERFACE_AK8_FACTORY_H_
