OPT=-O3
ROOT=`root-config --cflags --glibs`  -lRooFit -lRooFitCore
CFLAGS=-I${CMSSW_BASE}/src

.PHONY: mt-sf-measurement et-sf-measurement all test

all: mt-sf-measurement et-sf-measurement mm-sf-measurement

test: test-mt-sf-measurement test-et-sf-measurement test-mm-sf-measurement

mt-sf-measurement: plugins/mt-sf-measurement.cc
	g++ $(OPT) plugins/mt-sf-measurement.cc $(ROOT) $(CFLAGS) -o bin/mt-sf-measurement

mm-sf-measurement: plugins/mm-sf-measurement.cc
	g++ $(OPT) plugins/mm-sf-measurement.cc $(ROOT) $(CFLAGS) -o bin/mm-sf-measurement

et-sf-measurement: plugins/et-sf-measurement.cc
	g++ $(OPT) plugins/et-sf-measurement.cc $(ROOT) $(CFLAGS) -o bin/et-sf-measurement

test-mt-sf-measurement: plugins/mt-sf-measurement.cc
	g++ plugins/mt-sf-measurement.cc $(ROOT) $(CFLAGS) -o test

test-mm-sf-measurement: plugins/mm-sf-measurement.cc
	g++ plugins/mm-sf-measurement.cc $(ROOT) $(CFLAGS) -o test

test-et-sf-measurement: plugins/et-sf-measurement.cc
	g++ plugins/et-sf-measurement.cc $(ROOT) $(CFLAGS) -o test

clean:
	rm bin/*
