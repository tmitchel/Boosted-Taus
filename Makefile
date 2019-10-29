OPT=-O3
ROOT=`root-config --cflags --glibs`  -lRooFit -lRooFitCore
CFLAGS=-I${CMSSW_BASE}/src

mt-sf-measurement: plugins/mt-sf-measurement.cc
	g++ $(OPT) plugins/mt-sf-measurement.cc $(ROOT) $(CFLAGS) -o bin/mt-sf-measurement

et-sf-measurement: plugins/et-sf-measurement.cc
	g++ $(OPT) plugins/et-sf-measurement.cc $(ROOT) $(CFLAGS) -o bin/et-sf-measurement

clean:
	rm bin/*