import ROOT as r
from glob import glob


def get_histos(fout, fileList, variable):
    """Read all histograms from the input directory and pack
    them into a dictionary"""

    hists = {}
    for ifile in fileList:
        fin = r.TFile(ifile, 'READ')
        fout.cd()
        ihist = fin.Get(variable).Clone()
        hists[ifile.split('/')[-1].replace('.root', '')] = ihist
        fin.Close()
    return hists


def format_data(data, legend):
    """Make the data look pretty"""

    data.SetLineColor(r.kBlack)
    data.SetFillColor(0)
    data.SetMarkerStyle(8)
    legend.AddEntry(data, 'Data', 'lep')
    return data


def format_backgrounds(histos, legend):
    """Make all the backgrounds look pretty then build the
    stack and the stat uncertainty histogram"""

    histos['DYJets'].SetFillColor(r.TColor.GetColor("#f9cd66"))
    histos['SingleTop'].SetFillColor(r.TColor.GetColor("#9feff2"))
    histos['ttbar'].SetFillColor(r.TColor.GetColor("#cfe87f"))
    histos['WJets'].SetFillColor(r.TColor.GetColor("#ffccff"))
    histos['Diboson'].SetFillColor(r.TColor.GetColor("#de5a6a"))

    stack = r.THStack()
    stat = histos['DYJets'].Clone()
    stat.Reset()
    stat.SetName('stat')
    sorted_histos = sorted(histos.iteritems(), key=lambda kv: kv[1].Integral())
    for name, ihist in sorted_histos:
        stack.Add(ihist)
        stat.Add(ihist)
        legend.AddEntry(ihist, name, 'f')

    return stack, stat


def formatStat(stat):
    """Make the stat uncertainty histogram pretty"""

    stat.SetMarkerStyle(0)
    stat.SetLineWidth(2)
    stat.SetLineColor(0)
    stat.SetFillStyle(3004)
    stat.SetFillColor(r.kBlack)


def createCanvas():
    """Create the canvas with the right margins, etc."""

    can = r.TCanvas('can', 'can', 432, 451)
    can.Divide(2, 1)

    pad1 = can.cd(1)
    pad1.SetLeftMargin(.12)
    pad1.cd()
    pad1.SetPad(0, .3, 1, 1)
    pad1.SetTopMargin(.1)
    pad1.SetBottomMargin(0.02)
#    pad1.SetLogy()
    pad1.SetTickx(1)
    pad1.SetTicky(1)

    pad2 = can.cd(2)
    pad2.SetLeftMargin(.12)
    pad2.SetPad(0, 0, 1, .3)
    pad2.SetTopMargin(0.06)
    pad2.SetBottomMargin(0.35)
    pad2.SetTickx(1)
    pad2.SetTicky(1)

    can.cd(1)

    return can


def format_labels(can):
    """Draw LaTeX things on the canvas"""

    can.cd(1)
    ll = r.TLatex()
    ll.SetNDC(r.kTRUE)
    ll.SetTextSize(0.06)
    ll.SetTextFont(42)
    lepLabel = "#mu#tau_{#mu}"
    lumi = "41.5 fb^{-1}"
    ll.DrawLatex(0.42, 0.92, "{} {}, {} (13 TeV)".format(
        lepLabel, '2017', lumi))

    cms = r.TLatex()
    cms.SetNDC(r.kTRUE)
    cms.SetTextFont(61)
    cms.SetTextSize(0.09)
    cms.DrawLatex(0.16, 0.8, "CMS")

    prel = r.TLatex()
    prel.SetNDC(r.kTRUE)
    prel.SetTextFont(52)
    prel.SetTextSize(0.06)
    prel.DrawLatex(0.16, 0.74, "Preliminary")
    return ll, cms, prel


def formatRatio(data, stat):
    """Calculate the data/MC ratio then plot it nicely"""

    ratio = data.Clone()
    ratio.Divide(stat)
    ratio.SetTitle('')
    ratio.SetMaximum(2)
    ratio.SetMinimum(0)
    ratio.GetXaxis().SetTitle()
    ratio.SetMarkerStyle(21)
    ratio.GetXaxis().SetTitleSize(0.18)
    ratio.GetXaxis().SetTitleOffset(0.8)
    ratio.GetXaxis().SetTitleFont(42)
    ratio.GetXaxis().SetLabelFont(42)
    ratio.GetXaxis().SetLabelSize(.111)
    ratio.GetXaxis().SetNdivisions(505)

    ratio.GetYaxis().SetTitle('Data / MC')
    ratio.GetYaxis().SetTitleSize(0.12)
    ratio.GetYaxis().SetTitleFont(42)
    ratio.GetYaxis().SetTitleOffset(.475)
    ratio.GetYaxis().SetLabelSize(.12)
    ratio.GetYaxis().SetNdivisions(204)

    ratio_unc = ratio.Clone()
    for ibin in range(1, ratio_unc.GetNbinsX()+1):
        ratio_unc.SetBinContent(ibin, 1)
        ratio_unc.SetBinError(ibin, ratio.GetBinError(ibin))
    ratio_unc.SetMarkerSize(0)
    ratio_unc.SetMarkerStyle(8)
    ratio_unc.SetFillColor(r.kGray)

    return ratio, ratio_unc


def sigmaLines(data):
    """Draw lines on ratio plot"""

    low = data.GetBinLowEdge(1)
    high = data.GetBinLowEdge(data.GetNbinsX()) + \
        data.GetBinWidth(data.GetNbinsX())

    # high line
    line1 = r.TLine(low, 1.5, high, 1.5)
    line1.SetLineWidth(1)
    line1.SetLineStyle(3)
    line1.SetLineColor(r.kBlack)

    # low line
    line2 = r.TLine(low, 0.5, high, 0.5)
    line2.SetLineWidth(1)
    line2.SetLineStyle(3)
    line2.SetLineColor(r.kBlack)

    return line1, line2


def main(args):
    # get the input files
    filelist = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]
    fout = r.TFile('out.root', 'RECREATE')
    histos = get_histos(fout, filelist, args.variable)

    # create the histogram legend
    legend = r.TLegend(0.7, 0.7, 0.85, 0.85)
    legend.SetLineColor(0)
    legend.SetFillColor(0)
    legend.SetTextFont(61)
    legend.SetTextFont(42)
    legend.SetTextSize(0.03)

    data = format_data(histos.pop('Data'), legend)
    stack, stat = format_backgrounds(histos, legend)
    formatStat(stat)

    can = createCanvas()
    r.gStyle.SetOptStat(0)
    high = max(data.GetMaximum(), stat.GetMaximum()) * args.scale
    stack.SetMaximum(high)

    stack.Draw('hist')
    stat.Draw('same e2')
    data.Draw('lep same')
    legend.Draw('same')

    ll, cms, prel = format_labels(can)

    can.cd(2)

    ratio, ratio_unc = formatRatio(data, stat)
    ratio_unc.Draw('same e2')
    ratio.Draw('same lep')

    line1, line2 = sigmaLines(data)
    line1.Draw()
    line2.Draw()

    can.Print('file.pdf')


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input-dir', '-i', action='store',
                        dest='input_dir', help='path to input files')
    parser.add_argument('--var', '-v', action='store',
                        dest='variable', help='variable to plot')
    parser.add_argument('--scale', '-s', action='store',
                        default=1., type=float, help='scale the top by x')

    main(parser.parse_args())
