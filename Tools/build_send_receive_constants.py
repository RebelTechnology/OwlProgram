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

OWL_BUTTONS = ['Push', 'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8']
MIDI_RECEIVERS = ['__hv_midiin',
                  '__hv_notein',
                  '__hv_bendin',
                  '__hv_ctlin',
                  '__hv_pgmout']
MIDI_SENDERS = ['__hv_midiout',
                '__hv_noteout',
                '__hv_bendout',
                '__hv_ctlout',
                '__hv_pgmout']
               
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
    jdata = list()
    midiin = list()
    midiout = list()

    # Read receive and send object data
    with open(args.infilename, mode="r") as f:
        ir = json.load(f)

        for name, v in ir['control']['receivers'].iteritems():
            # # skip __hv_init and similar
            # if name.startswith("__"):
            #     continue

            # If a name has been specified
            if name in MIDI_RECEIVERS:
                midiin.append((name, "0x%x" % heavy_hash(name)))
                print "MIDI in "+name
            elif 'owl' in v['attributes'] and v['attributes']['owl'] is not None:
                key = v['attributes']['owl']
                jdata.append((key, name, 'RECV', "0x%x" % heavy_hash(name),
                              v['attributes']['min'],
                              v['attributes']['max'],
                              v['attributes']['default'],
                              key in OWL_BUTTONS))
            elif name.startswith('Channel-'):
                key = name.split('Channel-', 1)[1]
                jdata.append((key, name, 'RECV', "0x%x" % heavy_hash(name),
                              0, 1, 0.5, key in OWL_BUTTONS))

        for k, v in ir['objects'].iteritems():
            try:
                if v['type'] == '__send':
                    name = v['args']['name']
                    if name in MIDI_SENDERS:
                        midiout.append((name, "0x%x" % heavy_hash(name)))
                        print "MIDI out "+name
                    elif 'owl' in v['args']['attributes'] and v['args']['attributes']['owl'] is not None:
                        key = v['args']['attributes']['owl']
                        jdata.append((key, name+'>', 'SEND', "0x%x" % heavy_hash(name),
                                      v['args']['attributes']['min'],
                                      v['args']['attributes']['max'],
                                      v['args']['attributes']['default'],
                                      key in OWL_BUTTONS))
                    elif name.startswith('Channel-'):
                        key = name.split('Channel-', 1)[1]
                        jdata.append((key, name+'>', 'SEND', "0x%x" % heavy_hash(name),
                                      0, 1, 0.5, key in OWL_BUTTONS))
            except:
                pass

    # TODO, check that there is not channel defined both as input and output
                                                              
    # Write to files
    with open(args.outfilename, mode="w") as f:
        template = get_template("Heavy_owl_constants.tpl.h")
        outputText = template.render(jdata=jdata)  # this is where to put args to the template renderer
        f.write(outputText)

    HeavyPatchpath = "/".join(args.outfilename.split("/")[:-1]) + '/HeavyPatch.hpp'
    with open(HeavyPatchpath, mode="w") as f:
        template = get_template("HeavyPatch.tpl.hpp")
        outputText = template.render(jdata=jdata, midiin=midiin, midiout=midiout)  # this is where to put args to the template renderer
        f.write(outputText)

if __name__ == '__main__':
    main()
