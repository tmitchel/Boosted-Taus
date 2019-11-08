import build_qcd

class QCDer():
    def __init__(self, input_dir, variable):
        self.input_dir = input_dir
        self.variable = variable

print build_qcd.build_qcd(QCDer('ztt-sf-mt', ['Z_mass', 'Z_pt']), write_output=True)