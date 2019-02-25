# Boosted Tau Analysis

<p align="center"><img src="https://amva4newphysics.files.wordpress.com/2017/07/tausignature_trans.png" width="50%" title="Tau Cone" align='left'></p>
The Higgs decay to tau leptons provides an excellent handle to study the Higgs coupling to fermions. These couplings can be used to gain greater understanding of the nature of the Higgs boson as well as enabling searches for physics beyond the Standard Model. An especially interesting place to conduct these studies is in the region of phase space where the Higgs decays to significantly boosted tau leptons. In this region, the two taus can merge into a single jet leading to increased difficulty in identification. A dedicated strategy is required to properly handle these taus. This repository contains the tools needed to study Higgs bosons decaying to boosted taus along with numerous studies conducted in this region of phase space. <br/><br/><br/><br/>

## Analysis Backend
A common backend has been constructed for use in the studies. All physics objects are defined in headers files contained within the `interface` directory. These header files handle reading from `ggNtuples`, applying basic object selection, and returning objects containing all related information. When writing an analyzer, simply include headers for whichever objects you intend to use. All related data will be read and given to you to do the real analysis work.

### Using a physics object
Including a new phyiscs object in your analysis code is as easy as adding a single `#include` statement. For example, if you want to use electrons in your analysis, simply add
```
#include "interface/electron_factory.h"
```
This will include the `Electron` class as well as the `Electron_Factory`. The `Electron` class is the base class used to represent a single electron and all associated information. The `Electron_Factory` class is used to construct all `Electron`s in a single event after applying a loose default selection. In order to read data from the input ROOT file, the factory must have its member variables associated with TBranches. This is done in the constructor as follows
```
auto electron_factory = Electron_Factory(tree);
```
Now, the factory is ready to be used in the event loop. The factory is run once per event to take the information from the input TBranches and produce physics objects. The `Run_Factory()` function is used for this purpose. NOTE: you must do `Run_Factory()` _BEFORE_ accessing any information from the factory. The factory is run in the example below
```
electron_factory.Run_Factory();
```
The variable `electron_factory` now contains all electron-related data from the ggNtuple. The `Electron`s in the factory can be accessed with the member function `getElectrons()`, which will return a shared pointer the the vector of `Electron`s.

All physics objects are filled in the exact same way making their inclusion as simple as possible. Be aware, a very loose selection is applied on all objects while running the factory. If this selection is too tight for your needs, it can be adjusted in the corresponding class's `Run_Factory()` function.

### Creating histograms
The analysis backend comes with a simple class to hold histograms removing some boilerplate from your analysis code. (Note to self: Perhaps I can convert it to simply read JSON files when creating histograms. Then you can have a JSON file per analyzer and not have to make copies of the header). The `histManager` class is used for this task. Histograms are created inside the `hists` map and linked to a user-defined key. These histograms are initialized when a new `histManager` is constructed, as shown below
```
auto hists = std::make_shared<histManager>(output_name);
```
The `output_name` is used to create an output ROOT file with the corresponding name. 

The `histManager` provides functions for filling histograms in the map. This is the only way I can find to keep the histograms associated with the output file and available for writing in the end. One benefit is that extra functions are included for common cases beyond a simple fill. One example is the function, `FillPrevBins`. This function's usage is shown below
```
hists->FillPrevBins("lead_gen_jet_eff", gen_jets->at(0).getPt(), 1.);
```
Calling this function will fill find the bin in which `gen_jets->at(0).getPt()` belongs. Then, it will increase the bin content of all bins up to the selected bin by the value 1. This is especially useful when calculating efficiency as a function of some variable (here as a function of generator-level jet pT).

All histograms in the `histManager` can be written to the associated file with the `Write()` function.
```
hists->Write();
```
The histograms will all be written in the root directory of the TFile, then the TFile will be closed.


## To-Do:
- [ ] add all variables to the classes
- [ ] complete initial studies
- [ ] enable user to pass JSON file to histManager

