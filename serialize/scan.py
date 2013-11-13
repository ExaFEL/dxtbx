#!/usr/bin/env python
#
# dxtbx.serialize.scan.py
#
#  Copyright (C) 2013 Diamond Light Source
#
#  Author: James Parkhurst
#
#  This code is distributed under the BSD license, a copy of which is
#  included in the root directory of this package.
from __future__ import division


def to_dict(scan):
    """Convert the scan model to a dictionary

    Params:
        scan The scan model

    Returns:
        A dictionary of the parameters

    """
    from collections import OrderedDict

    if scan == None:
        return None

    return OrderedDict(
        [
            ("image_range", scan.get_image_range()),
            ("oscillation", scan.get_oscillation()),
            ("exposure_time", list(scan.get_exposure_times())),
            ("epochs", list(scan.get_epochs())),
        ]
    )


def from_dict(d, t=None):
    """Convert the dictionary to a scan model

    Params:
        d The dictionary of parameters
        t The template dictionary to use

    Returns:
        The scan model

    """
    from dxtbx.model import Scan
    from scitbx.array_family import flex

    # If None, return None
    if d == None:
        if t == None:
            return None
        else:
            return from_dict(t, None)
    elif t != None:
        d = dict(t.items() + d.items())

    if not isinstance(d["exposure_time"], list):
        d["exposure_time"] = [d["exposure_time"]]

    # Check the number of epochs set and expand if necessary
    numi = d["image_range"][1] - d["image_range"][0] + 1
    nume = len(d["epochs"])
    numex = len(d["exposure_time"])
    if numi > 2:
        if nume == 2:
            diff = d["epochs"][1] - d["epochs"][0]
            offs = d["epochs"][1]
            d["epochs"] += [offs + (i + 1) * diff for i in range(numi - 2)]
        elif nume != numi:
            raise RuntimeError("Num epochs does not match num images")
    elif nume != numi:
        raise RuntimeError("Num epochs does not match num images")

    if numex < numi:
        if numex > 0:
            exposure_times = d["exposure_time"]
            exposure_times.extend(flex.double(numi - numex, exposure_times[-1]))
            d["exposure_time"] = exposure_times
        else:
            exposure_times = flex.double(numi, 0.0)
            d["exposure_time"] = exposure_times

    # Create the model from the dictionary
    return Scan(
        tuple(d["image_range"]),
        tuple(d["oscillation"]),
        flex.double(d["exposure_time"]),
        flex.double(d["epochs"]),
    )