OPT=-03
ROOT=`root-config --cflags --glibs`  -lRooFit -lRooFitCore

tester: plugins/mt-sf-measurement.cc
	g++ $(OPT) plugins/mt-sf-measurement.cc $(ROOT) -I ${CMSSW_BASE}/src -o mt-sf-measurement