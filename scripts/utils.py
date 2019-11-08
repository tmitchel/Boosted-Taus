import numpy
import uproot
import uproot_methods

def from_array(name, content, variances, edges):

    class TH1(uproot_methods.classes.TH1.Methods, list):
        pass

    class TAxis(object):
        def __init__(self, edges):
            self._fNbins = len(edges) - 1
            self._fXmin = edges[0]
            self._fXmax = edges[-1]
            if numpy.array_equal(edges, numpy.linspace(self._fXmin, self._fXmax, len(edges), dtype=edges.dtype)):
                self._fXbins = numpy.array([], dtype=">f8")
            else:
                self._fXbins = edges.astype(">f8")

    out = TH1.__new__(TH1)
    out._fXaxis = TAxis(edges)

    centers = (edges[:-1] + edges[1:]) / 2.0
    out._fEntries = out._fTsumw = out._fTsumw2 = content.sum()
    out._fTsumwx = (content * centers).sum()
    out._fTsumwx2 = (content * centers**2).sum()

    out._fTitle = name

    out._classname, content = b"TH1F", content.astype(">f4")

    valuesarray = numpy.empty(len(content) + 2, dtype=content.dtype)
    valuesarray[1:-1] = content
    valuesarray[0] = 0
    valuesarray[-1] = 0
    valuesarray = map(lambda x: 0. if x < 0 else x, valuesarray)

    varsarray = numpy.empty(len(content) + 2, dtype=content.dtype)
    varsarray[1:-1] = variances
    varsarray[0] = 0
    varsarray[-1] = 0

    out.extend(valuesarray)
    out._fSumw2 = varsarray

    return out