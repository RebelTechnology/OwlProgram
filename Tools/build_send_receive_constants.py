#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

import argparse
import json
from collections import OrderedDict as odict

import os
dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append('%s/hvcc' % dir_path)
import core.hv2ir.HeavyLangObject as HeavyLangObject

heavy_hash = HeavyLangObject.HeavyLangObject.get_hash

import jinja2

def get_template(path):
    templateLoader = jinja2.FileSystemLoader(searchpath=dir_path)
    templateEnv = jinja2.Environment(loader=templateLoader)
    template = templateEnv.get_template(path)
    return template

def main():
    parser = argparse.ArgumentParser(
        description="Generates the constants file for send_receive puredata objects.")

    parser.add_argument(
        "infilename",
        help="Input ir file",
    )

    parser.add_argument(
        "outfilename",
        help="Output file",
        default="Heavy_owl_constants.h"
    )

    args = parser.parse_args()

    channels = ['A', 'B', 'C', 'D','E', 'F', 'G', 'H',
                'AA', 'AB', 'AC', 'AD','AE', 'AF', 'AG', 'AH',
                'BA', 'BB', 'BC', 'BD','BE', 'BF', 'BG', 'BH',
                ]

    recvs = odict()
    sends = odict()
    mins = odict()
    maxs = odict()
    defs = odict()
    #from pprint import pprint as pp

    # Read receive and send object data
    with open(args.infilename, mode="r") as f:
        ir = json.load(f)

        for k, v in ir['control']['receivers'].iteritems():
            # skip __hv_init and similar
            if k.startswith("__"):
                continue

            # If a name has been specified
            if 'owl' in v['attributes'] and v['attributes']['owl'] is not None:
                key = v['attributes']['owl']
                recvs[key] = k
                mins[key] = v['attributes']['min']
                maxs[key] = v['attributes']['max']
                defs[key] = v['attributes']['default']
            elif k.startswith('Channel-'):
                key = k.split('Channel-', 1)[1]
                recvs[key] = k
                mins[key] = 0
                maxs[key] = 1
                defs[key] = 0.5

        for k, v in ir['objects'].iteritems():
            try:
                if v['type'] == '__send':
                    if 'owl' in v['args']['attributes'] and v['args']['attributes']['owl'] is not None:
                        key = v['args']['attributes']['owl']
                        sends[key] = v['args']['name']
                        mins[key] = v['args']['attributes']['min']
                        maxs[key] = v['args']['attributes']['max']
                        defs[key] = v['args']['attributes']['default']
                    else:
                        key = v['args']['name']
                        sends[key] = key
                        mins[key] = 0
                        maxs[key] = 1
                        defs[key] = 0.5
            except:
                pass

    # TODO, check that there is not channel defined both as input and output

    # Prepare data for output reordering by channel order
    jdata = list()
    with open(args.outfilename, mode="w") as f:
        for chan in channels:

            if chan in recvs:
                name = recvs[chan]
                typ = 'RECV'
                namehash = heavy_hash(name)
                minvalue = mins[chan]
                maxvalue = maxs[chan]
                defvalue = defs[chan]

            elif chan in sends:
                # first we compute the hash, then append the '>' for the label
                name = sends[chan]
                namehash = heavy_hash(name)
                name += '>'
                typ = 'SEND'
                minvalue = mins[chan]
                maxvalue = maxs[chan]
                defvalue = defs[chan]
            else:
                continue

            jdata.append((chan, name, typ, "0x%x" % namehash, minvalue, maxvalue, defvalue))

    # Write to files
    with open(args.outfilename, mode="w") as f:
        template = get_template("Heavy_owl_constants.tpl.h")
        outputText = template.render(jdata=jdata)  # this is where to put args to the template renderer
        f.write(outputText)

    HeavyPatchpath = "/".join(args.outfilename.split("/")[:-1]) + '/HeavyPatch.hpp'
    with open(HeavyPatchpath, mode="w") as f:
        template = get_template("HeavyPatch.tpl.hpp")
        outputText = template.render(jdata=jdata)  # this is where to put args to the template renderer
        f.write(outputText)

if __name__ == '__main__':
    main()
