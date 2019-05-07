import ROOT


def main(args):
    fin = ROOT.TFile(args.input, 'READ')
    fout = ROOT.TFile('trigger_efficiency.root', 'RECREATE')
    mass_num = fin.Get('mass_turnon').Clone()
    mass_den = fin.Get('mass_turnon_den').Clone()
    pt_num = fin.Get('pt_turnon').Clone()
    pt_den = fin.Get('pt_turnon_den').Clone()

    mass_eff = mass_num.Divide(mass_den)
    pt_eff = pt_num.Divide(pt_den)

    mass_bins = mass_eff.GetNbinsX()
    pt_bins = pt_eff.GetNbinsX()
    eff_map = ROOT.TH2F('trigger_efficiency', 'trigger_efficiency',
                        mass_bins, mass_eff.GetBinLowEdge(1), mass_eff.GetBinLowEdge(mass_bins + 1),
                        pt_bins, pt_eff.GetBinLowEdge(1), pt_eff.GetBinLowEdge(pt_bins + 1)
                        )

    for mbin in range(0, mass_bins + 1):
        for pbin in range(0, pt_bins + 1):
            mcontent = mass_eff.GetBinContent(mbin)
            pcontent = pt_eff.GetBinContent(pbin)
            eff_map.SetBinContent(mbin, pbin, mcontent * pcontent)

    fout.cd()
    eff_map.Write()
    fout.Close()


if __name__ == "__main__":
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument('-i', '--input', action='store', dest='input', help='name of input file')
    main(parser.parse_args())
