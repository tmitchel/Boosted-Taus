// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_FACTORY_H_
#define INTERFACE_GEN_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "TTree.h"

class Gen;
typedef std::vector<Gen> VGen;

class Gen_Factory {
   public:
    Gen_Factory(TTree *, bool);
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
    Bool_t is_data;
    Int_t nMC, nGoodGen;
    Float_t genMET, genMETPhi;
    VGen gen_particles, gen_jets;
    TLorentzVector MET_p4;
    std::vector<Int_t> *mcPID, *mcGMomPID, *mcMomPID, *mcParentage, *mcStatus;
    std::vector<Float_t> *mcPt, *mcMass, *mcEta, *mcPhi, *mcMomPt, *mcMomMass, *mcMomEta, *mcMomPhi;
    std::vector<Float_t> *jetGenJetPt, *jetGenJetEta, *jetGenJetPhi, *jetGenJetEn;
};

Gen_Factory::Gen_Factory(TTree *tree, bool is_data_)
    : is_data(is_data_),
      mcPID(nullptr),
      mcPt(nullptr),
      mcMass(nullptr),
      mcEta(nullptr),
      mcPhi(nullptr),
      mcGMomPID(nullptr),
      mcMomPID(nullptr),
      mcMomPt(nullptr),
      mcMomMass(nullptr),
      mcMomEta(nullptr),
      mcMomPhi(nullptr),
      mcParentage(nullptr),
      mcStatus(nullptr),
      jetGenJetPt(nullptr),
      jetGenJetEta(nullptr),
      jetGenJetPhi(nullptr),
      jetGenJetEn(nullptr) {
    if (!is_data) {
        tree->SetBranchAddress("nMC", &nMC);
        tree->SetBranchAddress("mcPID", &mcPID);
        tree->SetBranchAddress("mcPt", &mcPt);
        tree->SetBranchAddress("mcMass", &mcMass);
        tree->SetBranchAddress("mcEta", &mcEta);
        tree->SetBranchAddress("mcPhi", &mcPhi);
        tree->SetBranchAddress("mcGMomPID", &mcGMomPID);
        tree->SetBranchAddress("mcMomPID", &mcMomPID);
        tree->SetBranchAddress("mcMomPt", &mcMomPt);
        tree->SetBranchAddress("mcMomMass", &mcMomMass);
        tree->SetBranchAddress("mcMomEta", &mcMomEta);
        tree->SetBranchAddress("mcMomPhi", &mcMomPhi);
        tree->SetBranchAddress("mcParentage", &mcParentage);
        tree->SetBranchAddress("mcStatus", &mcStatus);

        // gen jets
        tree->SetBranchAddress("jetGenJetPt", &jetGenJetPt);
        tree->SetBranchAddress("jetGenJetEta", &jetGenJetEta);
        tree->SetBranchAddress("jetGenJetPhi", &jetGenJetPhi);
        tree->SetBranchAddress("jetGenJetEn", &jetGenJetEn);

        // gen MET
        tree->SetBranchAddress("genMET", &genMET);
        tree->SetBranchAddress("genMETPhi", &genMETPhi);
    }
}

void Gen_Factory::Run_Factory() {
    if (is_data) {
        return;
    }
    // get gen quarks (mc*)
    gen_particles.clear();
    for (auto i = 0; i < nMC; i++) {
        auto gen = Gen(mcPt->at(i), mcEta->at(i), mcPhi->at(i), mcMass->at(i));
        gen.setMomP4(mcMomPt->at(i), mcMomEta->at(i), mcMomPhi->at(i), mcMomMass->at(i));
        gen.PID = mcPID->at(i);
        gen.GMomPID = mcGMomPID->at(i);
        gen.MomPID = mcMomPID->at(i);
        gen.Parentage = mcParentage->at(i);
        gen.Status = mcStatus->at(i);
        gen_particles.push_back(gen);
    }

    // get gen jets (jetGenJet*)
    gen_jets.clear();
    for (auto i = 0; i < jetGenJetPt->size(); i++) {
        if (jetGenJetPt->at(i) == -999) {
            continue;
        }
        auto temp = Gen();
        temp.setP4WithEnergy(jetGenJetPt->at(i), jetGenJetEta->at(i), jetGenJetPhi->at(i), jetGenJetEn->at(i));
        gen_jets.push_back(temp);
    }

    // sort by pT
    std::sort(gen_jets.begin(), gen_jets.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() > p2.getPt(); });
    std::sort(gen_particles.begin(), gen_particles.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() > p2.getPt(); });

    // set generator-level information
    nGoodGen = gen_particles.size();
    MET_p4.SetPtEtaPhiM(genMET, 0, genMETPhi, 0);
}

class Gen {
    friend class Gen_Factory;

   public:
    Gen(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _mass) { this->p4.SetPtEtaPhiM(_pt, _eta, _phi, _mass); }
    Gen() { this->p4.SetPtEtaPhiM(0, 0, 0, 0); }

    // getters
    Int_t getPID() const { return PID; }
    Int_t getMomPID() const { return MomPID; }
    Int_t getGMomPID() const { return GMomPID; }
    Int_t getStatus() const { return Status; }
    Int_t getParentage() const { return Parentage; }
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    TLorentzVector getMomP4() { return MomP4; }
    Float_t getMomPt() const { return MomP4.Pt(); }
    Float_t getMomEta() const { return MomP4.Eta(); }
    Float_t getMomPhi() const { return MomP4.Phi(); }
    Float_t getMomMass() const { return MomP4.M(); }

    // setters
    void setMomP4(float _pt, float _eta, float _phi, float _mass) { MomP4.SetPtEtaPhiM(_pt, _eta, _phi, _mass); }
    void setP4WithEnergy(float _pt, float _eta, float _phi, float _energy) { p4.SetPtEtaPhiE(_pt, _eta, _phi, _energy); }

   private:
    TLorentzVector p4, MomP4;
    Int_t PID, GMomPID, MomPID, Parentage, Status;
};

#endif  // INTERFACE_GEN_FACTORY_H_
