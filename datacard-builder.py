import ROOT
from glob import glob
from pprint import pprint


def grab(fin, prefix, var, rebin=None):
    ihist = fin.Get(prefix + var).Clone()
    if rebin != None:
        ihist.Rebin(rebin)
    return ihist


def get_histos(fout, fileList, variable, rebin=None):
    """Read all histograms from the input directory and pack
    them into a dictionary"""

    hists = {}
    os_id_iso, ss_id_iso, os_antiid_antiiso, ss_antiid_antiiso = {}, {}, {}, {}
    os_antiid_iso, ss_antiid_iso, os_id_antiiso, ss_id_antiiso = {}, {}, {}, {}
    os_id_ctr, ss_id_ctr, os_antiid_ctr, ss_antiid_ctr = {}, {}, {}, {}
    for ifile in fileList:
        fin = ROOT.TFile(ifile, 'READ')
        fout.cd()

        name = ifile.split('/')[-1].replace('.root', '')
        if name == 'Data_output':
          name = 'Data'
        print name
        hists[ifile.split('/')[-1].replace('.root', '')] = grab(fin, 'OS_pass', variable, rebin)
        os_id_iso[name] = grab(fin, 'OS_pass', variable, rebin)
        ss_id_iso[name] = grab(fin, 'SS_pass', variable, rebin)
        os_antiid_antiiso[name] = grab(fin, 'OS_anti_fail', variable, rebin)
        ss_antiid_antiiso[name] = grab(fin, 'SS_anti_fail', variable, rebin)
        os_antiid_iso[name] = grab(fin, 'OS_anti_pass', variable, rebin)
        ss_antiid_iso[name] = grab(fin, 'SS_anti_pass', variable, rebin)
        os_id_antiiso[name] = grab(fin, 'OS_fail', variable, rebin)
        ss_id_antiiso[name] = grab(fin, 'SS_fail', variable, rebin)
        os_id_ctr[name] = grab(fin, 'OS_control', variable, rebin)
        ss_id_ctr[name] = grab(fin, 'SS_control', variable, rebin)
        os_antiid_ctr[name] = grab(fin, 'OS_anti_control', variable, rebin)
        ss_antiid_ctr[name] = grab(fin, 'SS_anti_control', variable, rebin)
        fin.Close()
    return hists, {
        'os_id_iso': os_id_iso,
        'ss_id_iso': ss_id_iso,
        'os_antiid_antiiso': os_antiid_antiiso,
        'ss_antiid_antiiso': ss_antiid_antiiso,
        'os_antiid_iso': os_antiid_iso,
        'ss_antiid_iso': ss_antiid_iso,
        'os_id_antiiso': os_id_antiiso,
        'ss_id_antiiso': ss_id_antiiso,
        'os_id_ctr': os_id_ctr,
        'ss_id_ctr': ss_id_ctr,
        'os_antiid_ctr': os_antiid_ctr,
        'ss_antiid_ctr': ss_antiid_ctr,

    }


def build_qcd(osss_histos):
    # copy the data histograms
    os_id_iso = osss_histos['os_id_iso']['Data'].Clone()
    ss_id_iso = osss_histos['ss_id_iso']['Data'].Clone()
    os_antiid_antiiso = osss_histos['os_antiid_antiiso']['Data'].Clone()
    ss_antiid_antiiso = osss_histos['ss_antiid_antiiso']['Data'].Clone()
    os_antiid_iso = osss_histos['os_antiid_iso']['Data'].Clone()
    ss_antiid_iso = osss_histos['ss_antiid_iso']['Data'].Clone()
    os_id_antiiso = osss_histos['os_id_antiiso']['Data'].Clone()
    ss_id_antiiso = osss_histos['ss_id_antiiso']['Data'].Clone()
    os_id_ctr = osss_histos['os_id_ctr']['Data'].Clone()
    ss_id_ctr = osss_histos['ss_id_ctr']['Data'].Clone()
    os_antiid_ctr = osss_histos['os_antiid_ctr']['Data'].Clone()
    ss_antiid_ctr = osss_histos['ss_antiid_ctr']['Data'].Clone()

    # do data - bkgs
    bkgs = ['DYJets125', 'SingleTop', 'ttbar', 'WJets', 'Diboson']
    for bkg in bkgs:
        os_id_iso.Add(osss_histos['os_id_iso'][bkg], -1)
        ss_id_iso.Add(osss_histos['ss_id_iso'][bkg], -1)
        os_antiid_antiiso.Add(osss_histos['os_antiid_antiiso'][bkg], -1)
        ss_antiid_antiiso.Add(osss_histos['ss_antiid_antiiso'][bkg], -1)
        os_antiid_iso.Add(osss_histos['os_antiid_iso'][bkg], -1)
        ss_antiid_iso.Add(osss_histos['ss_antiid_iso'][bkg], -1)
        os_id_antiiso.Add(osss_histos['os_id_antiiso'][bkg], -1)
        ss_id_antiiso.Add(osss_histos['ss_id_antiiso'][bkg], -1)
        os_id_ctr.Add(osss_histos['os_id_ctr'][bkg], -1)
        ss_id_ctr.Add(osss_histos['ss_id_ctr'][bkg], -1)
        os_antiid_ctr.Add(osss_histos['os_antiid_ctr'][bkg], -1)
        ss_antiid_ctr.Add(osss_histos['ss_antiid_ctr'][bkg], -1)

    nom_osss_ratio = os_antiid_iso.Integral() / ss_antiid_iso.Integral()
    antiiso_osss_ratio = os_antiid_antiiso.Integral() / ss_antiid_antiiso.Integral()
    control_osss_ratio = os_antiid_ctr.Integral() / ss_antiid_ctr.Integral()

    nom_norm = ss_id_iso.Clone()
    antiiso_norm = ss_id_antiiso.Clone()
    control_norm = ss_id_ctr.Clone()

    nom_norm.Scale(nom_osss_ratio)
    antiiso_norm.Scale(antiiso_osss_ratio)
    control_norm.Scale(control_osss_rati()

    nom_qcd = ss_id_antiiso.Clone()
    antiiso_qcd = ss_id_iso.Clone()
    control_qcd = ss_id_ctr.Clone9)

    nom_qcd.Scale(nom_norm.Integral() / nom_qcd.Integral())
    antiiso_qcd.Scale(antiiso_norm.Integral() / antiiso_qcd.Integral())
    control_qcd.Scale(control_norm.Integral() / control_qcd.Integral())

    for ibin in range(0, nom_qcd.GetNbinsX() + 1):
        if nom_qcd.GetBinContent(ibin) < 0:
            nom_qcd.SetBinContent(ibin, 0)

        if antiiso_qcd.GetBinContent(ibin) < 0:
            antiiso_qcd.SetBinContent(ibin, 0)

        if control_qcd.GetBinContent(ibin) < 0:
            control_qcd.SetBinContent(ibin, 0)

    return nom_qcd, antiiso_qcd, control_qcd


def main(args):
    # get the input files
    filelist = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]
    fout = ROOT.TFile('out.root', 'RECREATE')
    histos, osss_histos = get_histos(fout, filelist, '')
    histos['QCD'], osss_histos['os_id_antiiso']['QCD'], osss_histos['os_id_control']['QCD'] = build_qcd(osss_histos)

    # pass region
    fout.mkdir('pass')
    fout.cd('pass')
    data = histos.pop('Data')
    data.SetName('data_obs')
    data.Write()
    for name, histo in histos.iteritems():
        histo.SetName(name)
        histo.Write()

    # fail region
    fout.mkdir('fail')
    fout.cd('fail')
    antiiso_data = osss_histos['os_id_antiiso'].pop('Data')
    antiiso_data.SetName('data_obs')
    antiiso_data.Write()
    for name, histo in osss_histos['os_id_antiiso'].iteritems():
        histo.SetName(name)
        histo.Write()

    # Zmumu control region
    fout.mkdir('control')
    fout.cd('control')
    antiiso_data = osss_histos['os_id_control'].pop('Data')
    antiiso_data.SetName('data_obs')
    antiiso_data.Write()
    for name, histo in osss_histos['os_id_control'].iteritems():
        histo.SetName(name)
        histo.Write()

    fout.Close()


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input-dir', '-i', action='store',
                        dest='input_dir', help='path to input files')

    main(parser.parse_args())
