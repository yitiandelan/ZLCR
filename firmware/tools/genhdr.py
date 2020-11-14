'''
Copyright (c) 2016-2020, TIANLAN.tech
SPDX-License-Identifier: Apache-2.0

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
'''

import os
import sys
from fire import Fire
from io import StringIO


class genhdr:
    def __init__(self):
        self.c = ''
        self.path = ''

    def join(self, RootDir):
        assert os.path.isdir(RootDir)
        self.path = RootDir + '/py'
        sys.path.insert(0, self.path)

    def cflags(self, *cflag):
        pass

    def gen_modules(self, RootDir, OutFile, *Source):
        for fp in Source:
            assert os.path.isfile(fp)
        assert os.path.isdir(os.path.dirname(OutFile))
        self.join(RootDir)

        from makemoduledefs import find_module_registrations, generate_module_table_header
        from contextlib import redirect_stdout

        modules = set()
        for fp in Source:
            modules |= find_module_registrations(fp)
        fo = StringIO()
        with redirect_stdout(fo):
            generate_module_table_header(sorted(modules))
        with open(OutFile, 'w') as f:
            f.writelines(fo.getvalue())

    def gen_version(self, RootDir, OutFile):
        assert os.path.isdir(os.path.dirname(OutFile))
        self.join(RootDir)

        from makeversionhdr import make_version_header
        from contextlib import redirect_stdout

        os.chdir(RootDir)
        fo = StringIO()
        with redirect_stdout(fo):
            make_version_header(OutFile)
        fo.getvalue()

    def gen_qstr(self, RootDir, OutFile, WorkDir, Include='.', QSTR=''):
        assert os.path.isdir(os.path.dirname(OutFile))
        assert os.path.isdir(WorkDir)
        self.join(RootDir)

        OutDir = os.path.dirname(OutFile)
        Include = [Include, RootDir, WorkDir, '/'.join(OutDir.split('/')[:-1])]

        def f(x): return x.endswith(".c")

        fn = [f'{self.path}/{n}' for n in os.listdir(self.path) if f(n)]
        fn += [f'{WorkDir}/{n}' for n in os.listdir(WorkDir) if f(n)]
        fp = f'{OutDir}/qstr.i.last'

        command = f'arm-none-eabi-gcc -E -DNO_QSTR -DMICROPY_ROM_TEXT_COMPRESSION=1 -I{" -I".join(Include)} {" ".join(fn)}'
        os.system(f'{command} > {fp}')
        ft = f'{OutDir}/qstr.split'

        command = f'python {self.path}/makeqstrdefs.py split qstr {fp} {OutDir}/qstr {OutDir}/qstrdefs.collected.h'
        os.system(f'{command} > /dev/null')
        os.system(f'touch {ft} > /dev/null')
        ft = f'{OutDir}/qstrdefs.collected.h'

        command = f'python {self.path}/makeqstrdefs.py cat qstr _ {OutDir}/qstr {ft}'
        os.system(f'{command} > /dev/null')
        fp = f'{OutDir}/qstrdefs.preprocessed.h'

        command = f'cat {self.path}/qstrdefs.h {QSTR} {ft}'
        command += """| sed 's/^Q(.*)/"&"/'"""
        command += f'| arm-none-eabi-gcc -E -DNO_QSTR -DMICROPY_ROM_TEXT_COMPRESSION=1 -I{" -I".join(Include)} -'
        command += """| sed 's/^\\"\(Q(.*)\)\\"/\\1/'"""
        command += f' > {fp}'
        os.system(f'{command}')

        command = f'python {self.path}/makeqstrdata.py {fp} > {OutFile}'
        os.system(f'{command}')

    def gen_frozen(self, RootDir, OutFile, Files, QSTR_HEAD):
        assert os.path.isdir(os.path.dirname(OutFile))
        self.join(RootDir)

        command = f'python {RootDir}/tools/mpy-tool.py --freeze --qstr-header {QSTR_HEAD} -mlongint-impl none {Files}'
        command += f' > {OutFile}'
        os.system(f'{command}')


if __name__ == "__main__":
    Fire(genhdr)
