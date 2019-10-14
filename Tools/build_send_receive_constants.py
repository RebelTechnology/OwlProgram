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

    channels = ['Channel-A', 'Channel-B', 'Channel-C', 'Channel-D',
                'Channel-E', 'Channel-F', 'Channel-G', 'Channel-H',
                'Channel-AA', 'Channel-AB', 'Channel-AC', 'Channel-AD',
                'Channel-AE', 'Channel-AF', 'Channel-AG', 'Channel-AH',
                'Channel-BA', 'Channel-BB', 'Channel-BC', 'Channel-BD',
                'Channel-BE', 'Channel-BF', 'Channel-BG', 'Channel-BH',
                ]

    recvs = odict()
    sends = odict()
    mins = odict()
    maxs = odict()
    defs = odict()
    #from pprint import pprint as pp

    with open(args.infilename, mode="r") as f:
        ir = json.load(f)
        #pp(ir)
        #ir

        for k, v in ir['control']['receivers'].iteritems():
            # skip __hv_init and similar
            if k.startswith("__"):
                continue

            # If a name has been specified
            if 'owl_param' in v['attributes'] and v['attributes']['owl_param'] is not None:
                key = 'Channel-'+v['attributes']['owl_param']
                recvs[key] = k
                mins[key] = v['attributes']['owl_min']
                maxs[key] = v['attributes']['owl_max']
                defs[key] = v['attributes']['owl_default']
            else:
                recvs[k] = k
                mins[k] = 0
                maxs[k] = 1
                defs[k] = 0.5

        for k, v in ir['objects'].iteritems():
            try:
                if v['type'] == '__send':
                    key = 'Channel-'+v['args']['attributes']['owl_param']
                    sends[key] = v['args']['name']
                    mins[key] = v['args']['attributes']['owl_min']
                    maxs[key] = v['args']['attributes']['owl_max']
                    defs[key] = v['args']['attributes']['owl_default']
            except:
                pass


    #
    # TODO, check that there is not channel defined both as input and output


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

            const = chan.replace('-', '_')
            const = const.upper()

            f.write('#define HV_NAME_%s "%s"\n' % (const, name))
            f.write("#define HV_HASH_%s_%s 0x%x\n" % (typ, const, namehash))
            f.write('#define HV_MIN_%s %s\n' % (const, minvalue))
            f.write('#define HV_MAX_%s %s\n' % (const, maxvalue))
            f.write('#define HV_DEFAULT_%s %s\n' % (const, defvalue))

if __name__ == '__main__':
    main()
