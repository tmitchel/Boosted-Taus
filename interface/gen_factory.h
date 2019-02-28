// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_FACTORY_H_
#define INTERFACE_GEN_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./gen.h"
#include "TTree.h"

typedef std::vector<Gen> VGen;

class Gen_Factory {
 public:
  explicit Gen_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalGen() { return nMC; }
  Int_t getNGoodGen() { return nGoodGen; }
  Float_t getMET() { return genMET; }
  Float_t getMetPhi() { return genMETPhi; }
  TLorentzVector getMETP4() { return MET_p4; }
  std::shared_ptr<VGen> getGens() { return std::make_shared<VGen>(gen_particles); }
  std::shared_ptr<VGen> getGenJets() { return std::make_shared<VGen>(gen_jets); }

 private:
  Int_t nMC, nGoodGen;
  Float_t genMET, genMETPhi;
  VGen gen_particles, gen_jets;
  TLorentzVector MET_p4;
  std::vector<Int_t> *mcPID, *mcGMomPID, *mcMomPID, *mcParentage, *mcStatus;
  std::vector<Float_t> *mcVtx, *mcVty, *mcVtz, *mcPt, *mcMass, *mcEta, *mcPhi, *mcE, *mcEt, *mcGMomPID,
      *mcMomPID, *mcMomPt, *mcMomMass, *mcMomEta, *mcMomPhi, *mcParentage, *mcStatus, *mcCalIsoDR03,
      *mcCalIsoDR04, *mcTrkIsoDR03, *mcTrkIsoDR04;
  std::vector<Float_t> *jetGenJetPt, *jetGenJetEta, *jetGenJetPhi, *jetGenJetEn;
};

Gen_Factory::Gen_Factory(TTree *tree)
    : mcPID(nullptr),
      mcVtx(nullptr),
      mcVty(nullptr),
      mcVtz(nullptr),
      mcPt(nullptr),
      mcMass(nullptr),
      mcEta(nullptr),
      mcPhi(nullptr),
      mcE(nullptr),
      mcEt(nullptr),
      mcGMomPID(nullptr),
      mcMomPID(nullptr),
      mcMomPt(nullptr),
      mcMomMass(nullptr),
      mcMomEta(nullptr),
      mcMomPhi(nullptr),
      mcParentage(nullptr),
      mcStatus(nullptr),
      mcCalIsoDR03(nullptr),
      mcCalIsoDR04(nullptr),
      mcTrkIsoDR03(nullptr),
      mcTrkIsoDR04(nullptr),
      jetGenJetPt(nullptr),
      jetGenJetEta(nullptr),
      jetGenJetPhi(nullptr),
      jetGenJetEn(nullptr) {
  tree->SetBranchAddress("nMC", &nMC);
  tree->SetBranchAddress("mcPID", &mcPID);
  tree->SetBranchAddress("mcVtx", &mcVtx);
  tree->SetBranchAddress("mcVty", &mcVty);
  tree->SetBranchAddress("mcVtz", &mcVtz);
  tree->SetBranchAddress("mcPt", &mcPt);
  tree->SetBranchAddress("mcMass", &mcMass);
  tree->SetBranchAddress("mcEta", &mcEta);
  tree->SetBranchAddress("mcPhi", &mcPhi);
  tree->SetBranchAddress("mcE", &mcE);
  tree->SetBranchAddress("mcEt", &mcEt);
  tree->SetBranchAddress("mcGMomPID", &mcGMomPID);
  tree->SetBranchAddress("mcMomPID", &mcMomPID);
  tree->SetBranchAddress("mcMomPt", &mcMomPt);
  tree->SetBranchAddress("mcMomMass", &mcMomMass);
  tree->SetBranchAddress("mcMomEta", &mcMomEta);
  tree->SetBranchAddress("mcMomPhi", &mcMomPhi);
  tree->SetBranchAddress("mcParentage", &mcParentage);
  tree->SetBranchAddress("mcStatus", &mcStatus);
  tree->SetBranchAddress("mcCalIsoDR03", &mcCalIsoDR03);
  tree->SetBranchAddress("mcCalIsoDR04", &mcCalIsoDR04);
  tree->SetBranchAddress("mcTrkIsoDR03", &mcTrkIsoDR03);
  tree->SetBranchAddress("mcTrkIsoDR04", &mcTrkIsoDR04);

  tree->SetBranchAddress("jetGenJetPt", &jetGenJetPt);
  tree->SetBranchAddress("jetGenJetEta", &jetGenJetEta);
  tree->SetBranchAddress("jetGenJetPhi", &jetGenJetPhi);
  tree->SetBranchAddress("jetGenJetEn", &jetGenJetEn);

  tree->SetBranchAddress("genMET", &genMET);
  tree->SetBranchAddress("genMETPhi", &genMETPhi);
}

void Gen_Factory::Run_Factory() {
  // get gen quarks (mc*)
  gen_particles.clear();
  for (auto i = 0; i < nMC; i++) {
    auto gen = Gen(mcPt->at(i), mcEta->at(i), mcPhi->at(i), mcMass->at(i), false);
    gen.PID = mcPID->at(i);
    gen.Vtx = mcVtx->at(i);
    gen.Vty = mcVty->at(i);
    gen.Vtz = mcVtz->at(i);
    gen.E = mcE->at(i);
    gen.Et = mcEt->at(i);
    gen.GMomPID = mcGMomPID->at(i);
    gen.MomPID = mcMomPID->at(i);
    gen.MomPt = mcMomPt->at(i);
    gen.MomMass = mcMomMass->at(i);
    gen.MomEta = mcMomEta->at(i);
    gen.MomPhi = mcMomPhi->at(i);
    gen.Parentage = mcParentage->at(i);
    gen.Status = mcStatus->at(i);
    gen.CalIsoDR03 = mcCalIsoDR03->at(i);
    gen.CalIsoDR04 = mcCalIsoDR04->at(i);
    gen.TrkIsoDR03 = mcTrkIsoDR03->at(i);
    gen.TrkIsoDR04 = mcTrkIsoDR04->at(i);
    gen_particles.push_back(gen);
  }

  // get gen jets (jetGenJet*)
  gen_jets.clear();
  for (auto i = 0; i < jetGenJetPt->size(); i++) {
    if (jetGenJetPt->at(i) == -999) {
      continue;
    }
    gen_jets.push_back(Gen(jetGenJetPt->at(i), jetGenJetEta->at(i), jetGenJetPhi->at(i), jetGenJetEn->at(i), true));
  }

  // sort by pT
  std::sort(gen_jets.begin(), gen_jets.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() > p2.getPt(); });
  std::sort(gen_particles.begin(), gen_particles.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() > p2.getPt(); });

  // set generator-level information
  nGoodGen = gen_particles.size();
  MET_p4.SetPtEtaPhiM(genMET, 0, genMETPhi, 0);
}

#endif  // INTERFACE_GEN_FACTORY_H_
