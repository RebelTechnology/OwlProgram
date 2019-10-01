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
                ]

    recvs = odict()
    sends = odict()
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
            if 'owl_channel' in v['attributes'] and v['attributes']['owl_channel'] is not None:
                recvs[v['attributes']['owl_channel']] = k
            else:
                recvs[k] = k

        for k, v in ir['objects'].iteritems():
            try:
                if v['type'] == '__send':
                    sends[v['args']['attributes']['owl_channel']] = v['args']['name']
            except:
                pass


    #
    # TODO, aggiungere controllo perchè non ci sia un canale sia input che output

    with open(args.outfilename, mode="w") as f:
        for chan in channels:

            if chan in recvs:
                name = recvs[chan]
                typ = 'RECV'
                namehash = heavy_hash(name)

            elif chan in sends:
                # first we compute the hash, then append the '>' for the label
                name = sends[chan]
                namehash = heavy_hash(name)
                name += '>'
                typ = 'SEND'
            else:
                continue

            const = chan.replace('-', '_')
            const = const.upper()

            f.write('#define HV_NAME_%s "%s"\n' % (const, name))
            f.write("#define HV_HASH_%s_%s 0x%x\n" % (typ, const, namehash))


    recvs

if __name__ == '__main__':
    main()
