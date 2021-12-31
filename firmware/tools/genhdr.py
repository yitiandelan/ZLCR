#!/usr/bin/env python3
# Copyright (c) 2021 TIANLAN.tech
# SPDX-License-Identifier: Apache-2.0

# Language: Python

import os
from argparse import ArgumentParser
from inspect import getfullargspec


class genhdr:
    def __init__(self, RootDir):
        assert os.path.isdir(RootDir)
        self.path = RootDir

    def gen_modules(self, OutFile, *Source):
        RootDir = self.path
        OutDir = os.path.dirname(OutFile)

        assert os.path.isdir(OutDir)
        assert not False in (os.path.isfile(f) for f in Source)

        cmds = []
        cmds.append(f'''python {RootDir}/py/makemoduledefs.py \
                        {" ".join(Source)} \
                        > {OutFile}''')
        os.system("&&".join(cmds))

    def gen_version(self, OutFile):
        RootDir = self.path
        OutDir = os.path.dirname(OutFile)
        assert os.path.isdir(OutDir)

        cmds = []
        cmds.append(f'''cd {RootDir}''')
        cmds.append(f'''python py/makeversionhdr.py \
                        {OutFile}''')
        os.system("&&".join(cmds))

    def gen_qstr(self, OutFile, WorkDir, Include=".", QSTR=""):
        RootDir = self.path
        OutDir = os.path.dirname(OutFile)

        assert os.path.isdir(OutDir)
        assert os.path.isdir(WorkDir)
        assert os.path.isdir(Include)

        Include, Files = [Include], []
        Include += [RootDir, WorkDir, "{}/..".format(OutDir)]

        for p in ("{}/py".format(RootDir), WorkDir):
            for f in os.listdir(p):
                if not f.endswith(".c"):
                    continue
                Files.append("{}/{}".format(p, f))

        CC = "arm-none-eabi-gcc"
        cmds = []

        cmds.append(f'''{CC} -E \
                        -DNO_QSTR \
                        -DMICROPY_ROM_TEXT_COMPRESSION=1 \
                        {" ".join(["-I" + i for i in Include])} \
                        {" ".join(Files)} \
                        > {OutDir}/qstr.i.last''')

        cmds.append(f'''python {RootDir}/py/makeqstrdefs.py \
                        split \
                        qstr {OutDir}/qstr.i.last {OutDir}/qstr \
                        {OutDir}/qstrdefs.collected.h \
                        > /dev/null''')

        cmds.append(f'''touch {OutDir}/qstr.split \
                        > /dev/null''')

        cmds.append(f'''python {RootDir}/py/makeqstrdefs.py cat qstr _ {OutDir}/qstr {OutDir}/qstrdefs.collected.h \
                        > /dev/null''')
        cmds.append(f'''cat {RootDir}/py/qstrdefs.h {QSTR} {OutDir}/qstrdefs.collected.h \
                        | sed 's/^Q(.*)/"&"/' \
                        | {CC} -E \
                          -DNO_QSTR \
                          -DMICROPY_ROM_TEXT_COMPRESSION=1 \
                          {" ".join(["-I" + i for i in Include])} - \
                        | sed 's/^\\"\(Q(.*)\)\\"/\\1/' \
                        > {OutDir}/qstrdefs.preprocessed.h''')
        cmds.append(f'''python {RootDir}/py/makeqstrdata.py {OutDir}/qstrdefs.preprocessed.h \
                        > {OutFile}''')

        os.system("&&".join(cmds))

    def gen_frozen(self, OutFile, Files, QSTR_HEAD):
        RootDir = self.path
        OutDir = os.path.dirname(OutFile)
        assert os.path.isdir(OutDir)

        cmds = []
        cmds.append(f'''python {RootDir}/tools/mpy-tool.py \
                        --freeze \
                        --qstr-header {QSTR_HEAD} \
                        -mlongint-impl none {Files} \
                        > {OutFile}''')

        os.system("&&".join(cmds))


def main():
    p = ArgumentParser()

    p.add_argument("RootDir")
    s = p.add_subparsers(dest="method")

    for k, v in vars(genhdr).items():
        if not k.startswith("gen_"):
            continue
        t = s.add_parser(k)
        c = getfullargspec(v)
        for i, a in enumerate(c.args):
            if a == "self":
                continue
            elif c.defaults and len(c.args) - i <= len(c.defaults):
                d = c.defaults[i - len(c.args) + len(c.defaults)]
                t.add_argument("-{}".format(a), type=str, default=d)
            else:
                t.add_argument(a, type=str)
        if c.varargs:
            t.add_argument(c.varargs, type=str, nargs="+", default=[])

    args = p.parse_args()
    mods, func = genhdr(args.RootDir), args.method

    if func == "gen_modules":
        mods.gen_modules(args.OutFile, *args.Source)
    else:
        args = vars(args)
        args.pop("method")
        args.pop("RootDir")
        getattr(mods, func)(**args)


if __name__ == "__main__":
    main()
