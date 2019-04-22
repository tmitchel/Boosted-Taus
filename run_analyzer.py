import os
import subprocess
import multiprocessing


def format_command(args, ifile):
    if not os.path.exists(args.output_dir):
      os.mkdir(args.output_dir)
    output_name = ifile.replace('.root', '_output.root').split('/')[-1]
    print output_name
    if args.local:
      args.ext = ''
    callstring = './{} -i {}{} -o {}/{} -j {} -t {}'.format(args.exe, args.ext, ifile, args.output_dir, output_name, args.json, args.treename)
    if args.verbose:
      callstring += ' -v'
    return callstring


def run_command(cmd):
    print cmd
    return subprocess.call(cmd, shell=True)


def main(args):
    commands = []
    if args.local:
      file_list = [os.path.join(args.input_path, f) for f in os.listdir(args.input_path) if os.path.isfile(os.path.join(args.input_path, f))]
      commands = [format_command(args, ifile) for ifile in file_list]
    else:
      search = ['xrdfs', 'root://cmseos.fnal.gov/', 'ls', args.input_path]
      commands = [
          format_command(args, ifile) for ifile in subprocess.check_output(search).split('\n') if '.root' in ifile
      ]
   
    if args.parallel:
      # Use 4 cores if the machine has more than 8 total cores.
      # Otherwise, use half the available cores.
      n_processes = min(4, multiprocessing.cpu_count() / 2)
 
      pool = multiprocessing.Pool(processes=n_processes)
      r = pool.map_async(run_command, commands)
      r.wait()
    else:
      [run_command(command) for command in commands]

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(
        description='Run an executable on all files in a directory')
    parser.add_argument('--exe', '-e', action='store',
                        dest='exe', help='name of executable')
    parser.add_argument('--input-path', '-i', action='store',
                        dest='input_path', help='path to input')
    parser.add_argument('--local', '-l', action='store_true',
                        dest='local', help='is this a local file')
    parser.add_argument('--ext', '-x', action='store', dest='ext',
                        default='root://cmsxrootd.fnal.gov/', help='reader extension')
    parser.add_argument('--treename', '-t', action='store', dest='treename',
                        default='ggNtuplizer/EventTree', help='name of tree')
    parser.add_argument('--output-dir', action='store', dest='output_dir',
                        default='output', help='name of output directory')
    parser.add_argument('--json', '-j', action='store', dest='json',
                        default='configs/test.json', help='name of json config')
    parser.add_argument('--verbose', action='store_true', dest='verbose')
    parser.add_argument('--parallel', action='store_true', dest='parallel', help='run in multiprocess')

    main(parser.parse_args())
