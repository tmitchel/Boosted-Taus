// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_AK8_FACTORY_H_
#define INTERFACE_AK8_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./ak8.h"
#include "TTree.h"

typedef std::vector<AK8> VAK8;

class AK8_Factory {
   public:
    AK8_Factory(TTree *, bool);
    void Run_Factory();

    // getters
    Int_t getNTotalAK8() { return nJet; }
    Int_t getNGoodAK8() { return nGoodJet; }
    std::shared_ptr<VAK8> getAK8() { return std::make_shared<VAK8>(jets); }

   private:
    Bool_t is_data;
    Int_t nJet, nGoodJet;
    VAK8 jets;
    std::vector<Bool_t> *jetPFLooseId;
    std::vector<Int_t> *jetPartonID, *jetHadFlvr, *jetNCH, *jetNNP;
    std::vector<Float_t> *jetPt, *jetEta, *jetPhi, *jetEn, *jetRawPt, *jetRawEn, *jetP4Smear, *jetP4SmearUp,
        *jetP4SmearDo, *jetJECUnc, *jetCHF, *jetNHF, *jetCEF, *jetNEF, *jetMUF, *AK8JetPrunedMass;
};

AK8_Factory::AK8_Factory(TTree *tree, bool is_data_)
    : is_data(is_data_),
      jetPt(nullptr),
      jetEta(nullptr),
      jetPhi(nullptr),
      jetEn(nullptr),
      jetRawPt(nullptr),
      jetRawEn(nullptr),
      jetPartonID(nullptr),
      jetHadFlvr(nullptr),
      jetP4Smear(nullptr),
      jetP4SmearUp(nullptr),
      jetP4SmearDo(nullptr),
      jetPFLooseId(nullptr),
      jetJECUnc(nullptr),
      jetCHF(nullptr),
      jetNHF(nullptr),
      jetCEF(nullptr),
      jetNEF(nullptr),
      jetNCH(nullptr),
      jetNNP(nullptr),
      jetMUF(nullptr),
      AK8JetPrunedMass(nullptr) {
    tree->SetBranchAddress("nAK8Jet", &nJet);
    tree->SetBranchAddress("AK8JetPt", &jetPt);
    tree->SetBranchAddress("AK8JetEn", &jetEn);
    tree->SetBranchAddress("AK8JetEta", &jetEta);
    tree->SetBranchAddress("AK8JetPhi", &jetPhi);
    tree->SetBranchAddress("AK8JetRawPt", &jetRawPt);
    tree->SetBranchAddress("AK8JetRawEn", &jetRawEn);
    if (!is_data) {
        tree->SetBranchAddress("AK8JetPartonID", &jetPartonID);
        tree->SetBranchAddress("AK8JetHadFlvr", &jetHadFlvr);
        tree->SetBranchAddress("AK8JetP4Smear", &jetP4Smear);
        tree->SetBranchAddress("AK8JetP4SmearUp", &jetP4SmearUp);
        tree->SetBranchAddress("AK8JetP4SmearDo", &jetP4SmearDo);
    }
    tree->SetBranchAddress("AK8JetPFLooseId", &jetPFLooseId);
    tree->SetBranchAddress("AK8JetJECUnc", &jetJECUnc);
    tree->SetBranchAddress("AK8JetCHF", &jetCHF);
    tree->SetBranchAddress("AK8JetNHF", &jetNHF);
    tree->SetBranchAddress("AK8JetCEF", &jetCEF);
    tree->SetBranchAddress("AK8JetNEF", &jetNEF);
    tree->SetBranchAddress("AK8JetNCH", &jetNCH);
    tree->SetBranchAddress("AK8JetNNP", &jetNNP);
    tree->SetBranchAddress("AK8JetMUF", &jetMUF);
    tree->SetBranchAddress("AK8JetPrunedMass", &AK8JetPrunedMass);
}

void AK8_Factory::Run_Factory() {
    jets.clear();
    for (auto i = 0; i < nJet; i++) {
        if (jetPt->at(i) < 30 || fabs(jetEta->at(i)) > 3) {
            continue;
        }
        auto jet = AK8(jetPt->at(i), jetEta->at(i), jetPhi->at(i), jetEn->at(i));
        jet.RawPt = jetRawPt->at(i);
        jet.RawEn = jetRawEn->at(i);

        if (!is_data) {
            jet.PartonID = jetPartonID->at(i);
            jet.HadFlvr = jetHadFlvr->at(i);
        }
        jet.PFLooseId = jetPFLooseId->at(i);
        jet.JECUnc = jetJECUnc->at(i);
        jet.CHF = jetCHF->at(i);
        jet.NHF = jetNHF->at(i);
        jet.CEF = jetCEF->at(i);
        jet.NEF = jetNEF->at(i);
        jet.NCH = jetNCH->at(i);
        jet.NNP = jetNNP->at(i);
        jet.MUF = jetMUF->at(i);
        jet.PrunedMass = AK8JetPrunedMass->at(i);
        jets.push_back(jet);
    }

    // sort by pT
    std::sort(jets.begin(), jets.end(), [](AK8 &p1, AK8 &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodJet = jets.size();
}

#endif  // INTERFACE_AK8_FACTORY_H_
