#!/usr/bin/python

# Copyright 2015 Section6. All Rights Reserved.

import argparse
import getpass
import json
import os
import requests
import shutil
import stat
import tempfile
import time
import urlparse
import zipfile

class Colours:
    purple = "\033[95m"
    cyan = "\033[96m"
    dark_cyan = "\033[36m"
    blue = "\033[94m"
    green = "\033[92m"
    yellow = "\033[93m"
    red = "\033[91m"
    bold = "\033[1m"
    underline = "\033[4m"
    end = "\033[0m"

def __zip_dir(in_dir, zip_path, file_filter=None):
    zf = zipfile.ZipFile(zip_path, mode="w", compression=zipfile.ZIP_DEFLATED)
    for subdir, dirs, files in os.walk(in_dir):
        for file in files:
            if (file_filter is None) or (len(file_filter) > 0 and file.lower().split(".")[-1] in file_filter):
                zf.write(
                    filename=os.path.join(subdir,file),
                    arcname=os.path.relpath(os.path.join(subdir,file), start=in_dir))
    return zip_path

def __unzip(zip_path, target_dir):
    """Unzip a file to a given directory. All destination files are overwritten.
    """
    zipfile.ZipFile(zip_path).extractall(target_dir)

def main():
    parser = argparse.ArgumentParser(
        description="Compiles a Pure Data file.")
    parser.add_argument(
        "input_dir",
        help="A directory containing _main.pd. The entire directory will be uploaded.")
    parser.add_argument(
        "-n", "--name",
        default="heavy",
        help="Patch name. If it doesn't exist, the uploader will fail. Make sure that it exists on the Heavy website.")
    parser.add_argument(
        "-g", "--gen",
        nargs="+",
        default=["c"],
        help="List of generator outputs. Currently supported generators are 'c' and 'js'.")
    parser.add_argument(
        "-b",
        help="All files will be placed in the output directory, placed in their own subdirectory corresonding to the generator name.",
        action="count")
    parser.add_argument(
        "-o", "--out",
        nargs="+",
        default=["./"], # by default
        help="List of destination directories for retrieved files. Order should be the same as for --gen.")
    parser.add_argument(
        "-d", "--domain",
        default="https://enzienaudio.com",
        help="Domain. Default is https://enzienaudio.com.")
    parser.add_argument(
        "-x",
        help="Don't save the returned token.",
        action="count")
    parser.add_argument(
        "-z",
        help="Force the use of a password, regardless of saved token.",
        action="count")
    parser.add_argument(
        "--noverify",
        help="Don't verify the SSL connection. Generally a bad idea.",
        action="count")
    parser.add_argument(
        "-v", "--verbose",
        help="Show debugging information.",
        action="count")
    args = parser.parse_args()

    domain = args.domain or "https://enzienaudio.com"

    post_data = {}

    # token should be stored in ~/.heavy/token
    token_path = os.path.expanduser(os.path.join("~/", ".heavy", "token"))
    if os.path.exists(token_path) and not args.z:
        with open(token_path, "r") as f:
            post_data["credentials"] = {
                "token": f.read()
            }
    else:
        # otherwise, get the username and password
        post_data["credentials"] = {
            "username": raw_input("Enter username: "),
            "password": getpass.getpass("Enter password: ")
        }

    tick = time.time()

    # make a temporary directory
    temp_dir = tempfile.mkdtemp(prefix="lroyal-")

    # zip up the pd directory into the temporary directory
    try:
        if not os.path.exists(os.path.join(args.input_dir, "_main.pd")):
            raise Exception("Root Pd directory does not contain a file named _main.pd.")
        zip_path = __zip_dir(
            args.input_dir,
            os.path.join(temp_dir, "archive.zip"),
            file_filter={"pd"})
    except Exception as e:
        print e
        shutil.rmtree(temp_dir) # clean up the temporary directory
        return

    post_data["name"] = args.name

    # the outputs to generate (always include c)
    __SUPPORTED_GENERATOR_SET = {"c", "js"}
    post_data["gen"] = list(({"c"} | set(args.gen)) & __SUPPORTED_GENERATOR_SET)

    # upload the job, get the response back
    # NOTE(mhroth): multipart-encoded file can only be sent as a flat dictionary,
    # but we want to send a json encoded deep dictionary. So we do a bit of a hack.
    r = requests.post(
        urlparse.urljoin(domain, "/a/heavy"),
        data={"json":json.dumps(post_data)},
        files={"file": (os.path.basename(zip_path), open(zip_path, "rb"), "application/zip")},
        verify=False if args.noverify else True)

    if r.status_code != requests.codes.ok:
        shutil.rmtree(temp_dir) # clean up the temporary directory
        r.raise_for_status() # raise an exception

    # decode the JSON API response
    r_json = r.json()

    """
    {
      "data": {
        "compileTime": 0.05078411102294922,
        "id": "mhroth/asdf/Edp2G",
        "slug": "Edp2G",
        "index": 3,
        "links": {
          "files": {
            "linkage": [
              {
                "id": "mhroth/asdf/Edp2G/c",
                "type": "file"
              }
            ],
            "self": "https://enzienaudio.com/h/mhroth/asdf/Edp2G/files"
          },
          "project": {
            "linkage": {
              "id": "mhroth/asdf",
              "type": "project"
            },
            "self": "https://enzienaudio.com/h/mhroth/asdf"
          },
          "self": "https://enzienaudio.com/h/mhroth/asdf/Edp2G",
          "user": {
            "linkage": {
              "id": "mhroth",
              "type": "user"
            },
            "self": "https://enzienaudio.com/h/mhroth"
          }
        },
        "type": "job"
      },
      "included": [
        {
          "filename": "file.c.zip",
          "generator": "c",
          "id": "mhroth/asdf/Edp2G/c",
          "links": {
            "self": "https://enzienaudio.com/h/mhroth/asdf/Edp2G/c/file.c.zip"
          },
          "mime": "application/zip",
          "type": "file"
        }
      ],
      "warnings": [],
      "meta": {
        "token": "11AS0qPRmjTUHEMSovPEvzjodnzB1xaz"
      }
    }
    """
    reply_json = r.json()
    if args.verbose:
        print json.dumps(
            reply_json,
            sort_keys=True,
            indent=2,
            separators=(",", ": "))

    # update the api token, if present
    if "token" in reply_json.get("meta",{}) and not args.x:
        if not os.path.exists(os.path.dirname(token_path)):
            os.makedirs(os.path.dirname(token_path)) # ensure that the .heavy directory exists
        with open(token_path, "w") as f:
            f.write(reply_json["meta"]["token"])
        os.chmod(token_path, stat.S_IRUSR | stat.S_IWUSR) # force rw------- permissions on the file

    # print any warnings
    for x in r_json["warnings"]:
        print "{0}Warning:{1} {2}".format(Colours.yellow, Colours.end, x["detail"])

    # check for errors
    if len(r_json.get("errors",[])) > 0:
        shutil.rmtree(temp_dir) # clean up the temporary directory
        for x in r_json["errors"]:
            print "{0}Error:{1} {2}".format(Colours.red, Colours.end, x["detail"])
        return

    # retrieve all requested files
    for i,g in enumerate(args.gen):
        file_url = __get_file_url_for_generator(reply_json, g)
        if file_url is not None and (len(args.out) > i or args.b):
            r = requests.get(
                file_url,
                cookies={"token": reply_json["meta"]["token"]},
                verify=False if args.noverify else True)
            r.raise_for_status()

            # write the reply to a temporary file
            c_zip_path = os.path.join(temp_dir, "archive.{0}.zip".format(g))
            with open(c_zip_path, "wb") as f:
                f.write(r.content)

            # unzip the files to where they belong
            if args.b:
                target_dir = os.path.join(os.path.abspath(os.path.expanduser(args.out[0])), g)
            else:
                target_dir = os.path.abspath(os.path.expanduser(args.out[i]))
            if not os.path.exists(target_dir):
                os.makedirs(target_dir) # ensure that the output directory exists
            __unzip(c_zip_path, target_dir)

            print "{0} files placed in {1}".format(g, target_dir)
        else:
            print "{0}Warning:{1} {2} files could not be retrieved.".format(
                Colours.yellow, Colours.end,
                g)

    # delete the temporary directory
    shutil.rmtree(temp_dir)

    print "Job URL", reply_json["data"]["links"]["self"]
    print "Total request time: {0}ms".format(int(1000.0*(time.time()-tick)))

def __get_file_url_for_generator(json_api, g):
    """Returns the file link for a specific generator.
    Returns None if no link could be found.
    """
    for i in json_api["included"]:
        if g == i["generator"]:
            return i["links"]["self"]
    return None # by default, return None



if __name__ == "__main__":
    main()
