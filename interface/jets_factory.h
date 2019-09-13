// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_FACTORY_H_
#define INTERFACE_JETS_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./jets.h"
#include "TTree.h"

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
  std::vector<ULong64_t> *jetFiredTrgs;
  std::vector<Int_t> *jetLepTrackPID, *jetPartonID, *jetHadFlvr, *jetID, *jetPUFullID, *jetNCH, *jetNNP;
  std::vector<Float_t> *jetPt, *jetEta, *jetPhi, *jetEn, *jetRawPt, *jetRawEn, *jetMt, *jetArea, *jetLeadTrackPt, *jetLeadTrackEta, *jetLeadTrackPhi,
      *jetLepTrackPt, *jetLepTrackEta, *jetLepTrackPhi, *jetCSV2BJetTags, *jetDeepCSVTags_b, *jetDeepCSVTags_bb, *jetDeepCSVTags_c, *jetDeepCSVTags_udsg,
      *jetPUID, *jetJECUnc, *jetCHF, *jetNHF, *jetCEF, *jetNEF, *jetMUF;
};

Jets_Factory::Jets_Factory(TTree *tree, bool is_data_)
    : is_data(is_data_),
      jetPt(nullptr),
      jetEta(nullptr),
      jetPhi(nullptr),
      jetEn(nullptr),
      jetRawPt(nullptr),
      jetRawEn(nullptr),
      jetMt(nullptr),
      jetArea(nullptr),
      jetLeadTrackPt(nullptr),
      jetLeadTrackEta(nullptr),
      jetLeadTrackPhi(nullptr),
      jetLepTrackPID(nullptr),
      jetLepTrackPt(nullptr),
      jetLepTrackEta(nullptr),
      jetLepTrackPhi(nullptr),
      jetCSV2BJetTags(nullptr),
      jetDeepCSVTags_b(nullptr),
      jetDeepCSVTags_bb(nullptr),
      jetDeepCSVTags_c(nullptr),
      jetDeepCSVTags_udsg(nullptr),
      jetPartonID(nullptr),
      jetHadFlvr(nullptr),
      jetPFLooseId(nullptr),
      jetID(nullptr),
      jetPUID(nullptr),
      jetPUFullID(nullptr),
      jetJECUnc(nullptr),
      jetFiredTrgs(nullptr),
      jetCHF(nullptr),
      jetNHF(nullptr),
      jetCEF(nullptr),
      jetNEF(nullptr),
      jetNCH(nullptr),
      jetNNP(nullptr),
      jetMUF(nullptr) {
  tree->SetBranchAddress("nJet", &nJet);
  tree->SetBranchAddress("jetPt", &jetPt);
  tree->SetBranchAddress("jetEn", &jetEn);
  tree->SetBranchAddress("jetEta", &jetEta);
  tree->SetBranchAddress("jetPhi", &jetPhi);
  tree->SetBranchAddress("jetRawPt", &jetRawPt);
  tree->SetBranchAddress("jetRawEn", &jetRawEn);
  tree->SetBranchAddress("jetMt", &jetMt);
  tree->SetBranchAddress("jetArea", &jetArea);
  tree->SetBranchAddress("jetLeadTrackPt", &jetLeadTrackPt);
  tree->SetBranchAddress("jetLeadTrackEta", &jetLeadTrackEta);
  tree->SetBranchAddress("jetLeadTrackPhi", &jetLeadTrackPhi);
  tree->SetBranchAddress("jetLepTrackPID", &jetLepTrackPID);
  tree->SetBranchAddress("jetLepTrackPt", &jetLepTrackPt);
  tree->SetBranchAddress("jetLepTrackEta", &jetLepTrackEta);
  tree->SetBranchAddress("jetLepTrackPhi", &jetLepTrackPhi);
  tree->SetBranchAddress("jetCSV2BJetTags", &jetCSV2BJetTags);
  tree->SetBranchAddress("jetDeepCSVTags_b", &jetDeepCSVTags_b);
  tree->SetBranchAddress("jetDeepCSVTags_bb", &jetDeepCSVTags_bb);
  tree->SetBranchAddress("jetDeepCSVTags_c", &jetDeepCSVTags_c);
  tree->SetBranchAddress("jetDeepCSVTags_udsg", &jetDeepCSVTags_udsg);
  if (!is_data) {
    tree->SetBranchAddress("jetPartonID", &jetPartonID);
    tree->SetBranchAddress("jetHadFlvr", &jetHadFlvr);
  }
  tree->SetBranchAddress("jetPFLooseId", &jetPFLooseId);
  tree->SetBranchAddress("jetID", &jetID);
  tree->SetBranchAddress("jetPUID", &jetPUID);
  tree->SetBranchAddress("jetPUFullID", &jetPUFullID);
  tree->SetBranchAddress("jetJECUnc", &jetJECUnc);
  tree->SetBranchAddress("jetFiredTrgs", &jetFiredTrgs);
  tree->SetBranchAddress("jetCHF", &jetCHF);
  tree->SetBranchAddress("jetNHF", &jetNHF);
  tree->SetBranchAddress("jetCEF", &jetCEF);
  tree->SetBranchAddress("jetNEF", &jetNEF);
  tree->SetBranchAddress("jetNCH", &jetNCH);
  tree->SetBranchAddress("jetNNP", &jetNNP);
  tree->SetBranchAddress("jetMUF", &jetMUF);
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
    jet.RawPt = jetRawPt->at(i);
    jet.RawEn = jetRawEn->at(i);
    jet.Mt = jetMt->at(i);
    jet.Area = jetArea->at(i);
    jet.LeadTrackPt = jetLeadTrackPt->at(i);
    jet.LeadTrackEta = jetLeadTrackEta->at(i);
    jet.LeadTrackPhi = jetLeadTrackPhi->at(i);
    jet.LepTrackPID = jetLepTrackPID->at(i);
    jet.LepTrackPt = jetLepTrackPt->at(i);
    jet.LepTrackEta = jetLepTrackEta->at(i);
    jet.LepTrackPhi = jetLepTrackPhi->at(i);
    jet.CSV2BJetTags = jetCSV2BJetTags->at(i);
    jet.DeepCSVTags_b = jetDeepCSVTags_b->at(i);
    jet.DeepCSVTags_bb = jetDeepCSVTags_bb->at(i);
    jet.DeepCSVTags_c = jetDeepCSVTags_c->at(i);
    jet.DeepCSVTags_udsg = jetDeepCSVTags_udsg->at(i);
    if (!is_data) {
      jet.PartonID = jetPartonID->at(i);
      jet.HadFlvr = jetHadFlvr->at(i);
    }
    jet.PFLooseId = jetPFLooseId->at(i);
    jet.ID = jetID->at(i);
    jet.PUID = jetPUID->at(i);
    jet.PUFullID = jetPUFullID->at(i);
    jet.JECUnc = jetJECUnc->at(i);
    jet.FiredTrgs = jetFiredTrgs->at(i);
    jet.CHF = jetCHF->at(i);
    jet.NHF = jetNHF->at(i);
    jet.CEF = jetCEF->at(i);
    jet.NEF = jetNEF->at(i);
    jet.NCH = jetNCH->at(i);
    jet.NNP = jetNNP->at(i);
    jet.MUF = jetMUF->at(i);
    jets.push_back(jet);

    if (jetCSV2BJetTags->at(i)> 0.4941) {
      btags.push_back(jet);
    }
  }

  // sort by pT
  std::sort(jets.begin(), jets.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
  std::sort(btags.begin(), btags.end(), [](Jets &p1, Jets &p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodJet = jets.size();
  nBTag = btags.size();
}

#endif  // INTERFACE_JETS_FACTORY_H_
