// Copywrite [2019] Tyler Mitchell

#include "TFile.h"
#include "TTree.h"

int main() {
    auto fin = reinterpret_cast<TFile*>("inputFile.root", "READ");
    auto tree = reinterpret_cast<TTree*>(fin->Get("ggNtuplizer/EventTree"));

    Int_t nTau;
    tree->SetBranchAddress("nTau", &nTau);

    auto nevts = tree->getEntries();
    for (auto i = 0; i < nevts; i++) {
        tree->getEntry(i);
        std::cout << "Processing event " << i << " out of " << nevts << " events." << std::flush;

        for (auto itau = 0; itau < nTau; itau++) {
            std::cout << itau << std::endl;
        }
    }
    return 1;
}

