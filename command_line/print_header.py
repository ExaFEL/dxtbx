from __future__ import print_function


def print_header():
    import sys
    from dxtbx.format.Registry import Registry

    # this will do the lookup for every frame - this is strictly not needed
    # if all frames are from the same instrument

    for arg in sys.argv[1:]:
        format = Registry.find(arg)
        i = format(arg)
        print("Beam:")
        print(i.get_beam())
        print("Goniometer:")
        print(i.get_goniometer())
        print("Detector:")
        print(i.get_detector())
        print("Scan:")
        print(i.get_scan())
        print("Total Counts:")
        print(sum(i.get_raw_data()))


if __name__ == "__main__":
    print_header()