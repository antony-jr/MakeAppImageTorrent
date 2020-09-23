#!/usr/bin/env python3
import requests
import sys
import datetime
import os

# We will be using https://ngosang.github.io/trackerslist/trackers_best.txt
# as the source for the trackers.
trackerlist = "https://ngosang.github.io/trackerslist/trackers_best.txt"
directory = os.path.curdir

if len(sys.argv) != 1:
    directory = sys.argv[1]
    os.chdir(directory)

trackerhpp_a = '''////// Automatically Generated from 
////// https://ngosang.github.io/trackerslist/trackers_best.txt
#ifndef TRACKERS_HPP_INCLUDED
#define TRACKERS_HPP_INCLUDED
#include <string>
#include <vector>

namespace Trackers {
	const std::vector<std::string> All {
'''

trackerhpp_b = '''
        };
}

#endif // TRACKERS_HPP_INCLUDED
'''

print("-- Generating Tracker list: Trackers.hpp")

with open('Trackers.hpp', 'w') as fp:
    tracker_line = '                "{}"'
    fp.write("////// Generated on {}\n{}".format(datetime.datetime.now(), trackerhpp_a));

    try:
        response = requests.get(trackerlist)
        if response.status_code != 200:
            print("ERROR: Bad HTTP Response Code: {}".format(response.status_code) )
            sys.exit(-1)
    except Exception as e:
        print("ERROR: {}".format(e))
        sys.exit(-1)

    trackers = response.content.decode().split('\n')

    print_prev = False
    for tracker_link in trackers:
        if len(tracker_link) == 0:
            continue
        formatted = tracker_line.format(tracker_link)
        if print_prev:
            fp.write(',\n')
        fp.write(formatted)
        print_prev = True

    fp.write(trackerhpp_b);
    fp.close()
