#!/usr/bin/env python

# Noah Beach
# 11/02/2017
# CS 325 Fall 2017
# Group 33 Project
# Traveling Salesman Problem - Nearest Neighbor Algorithm

import sys, time, itertools, math, exceptions, copy
from random import shuffle

def openFile(fileName):
    try:
        f = open(fileName, "r+")
        return f
    except IOError:
        f = open(fileName, "w+")
        return f
    except Exception as e:
        print fileName + ' could not be opened: ' + str(e)

def parseFile(fileHandle):
    # Holder for cities
    cities = {}

    for line in fileHandle:
        try:
            # Input file is in the format:
            # cityID x-coord y-coord
            cityLine = line.split()
            #print cityLine
            if cityLine[0].isdigit():
                cities[int(cityLine[0])] = {}
                cities[int(cityLine[0])].update({"id": int(cityLine[0]),
                                                "xcoord": int(cityLine[1]),
                                                "ycoord": int(cityLine[2]),
                                                "visited": False})
        # End of file
        except Exception as e:
            pass
    return cities

def distanceBetween(citya, cityb):
    # Distance formula from Project Description
    return int(round(math.sqrt((citya["xcoord"] - cityb["xcoord"])**2 + (citya["ycoord"] - cityb["ycoord"])**2)))


def findNearestNeighbor(cityID, cities):
    # Set the current nearest city to invalid index, as well as set the distance
    # to infinity. float("inf") found here: https://stackoverflow.com/a/7781273
    nearest = {"cityID": -1, "distance": float("inf")}

    # Iterate over each city
    for i in xrange(len(cities)):
        # If city is current city or city has been visited, skip
        if cityID == i or cities[i]["visited"] == True:
            pass
        else:
            # Compute distance from current city to city
            distance = distanceBetween(cities[cityID], cities[i])
            # Update nearest with new nearest
            if distance < nearest["distance"]:
                nearest.update({"cityID": i, "distance": distance})
    # Mark the nearest city as visited, return nearest
    cities[nearest["cityID"]]["visited"] = True
    return nearest

def nearestNeighbor(cities, startCityID):
    tour = {"path": [], "distance": 0}

    # Add starting city as first city and mark it visited
    start = {"cityID": startCityID, "distance": 0}
    cities[startCityID]["visited"] = True
    tour["path"].append(start["cityID"])

    # Setup iterator, iterate over all cities moving along a path running
    # findNearestNeighbor on each, updating the path and incrementing distance
    next = start
    for i in xrange(len(cities) - 1):
        current = findNearestNeighbor(next["cityID"], cities)
        tour["path"].append(current["cityID"])
        tour["distance"] += current["distance"]
        next = current

    # Add the distance from the last city to the first city
    tour["distance"] += distanceBetween(cities[tour["path"][0]], cities[tour["path"][-1]])
    return tour

if __name__ == "__main__":
    # Try to open the file provided for reading and set up output file
    rfile = openFile(sys.argv[1])
    wfile = openFile(sys.argv[1] + ".tour")

    # Collect cities from file
    cities = parseFile(rfile)

    # Make a copy of cities to work with
    working_cities = copy.deepcopy(cities)

    # Initialize best_tour to be empty path and infinite distance
    best_tour = {"path": [], "distance": float("inf")}

    # If dealing with large amount of cities (more than 2000), only repeat 100
    # times to try and find a good solution, otherwise repeat "n" times
    if len(cities) <= 2000:
        repetition = len(cities)
    else:
        repetition = 100

    # Get list of city IDs from cities dictionary
    cityIDs = list(cities)

    # Run algorithm
    start = time.time()
    for i in xrange(repetition):
        current_tour = nearestNeighbor(working_cities, cityIDs.pop())
        if current_tour["distance"] < best_tour["distance"]:
            best_tour = current_tour
        working_cities = copy.deepcopy(cities)
    finish = time.time()

    wfile.write("%d\n" % best_tour["distance"])
    for city in best_tour["path"]:
        wfile.write("%d\n" % city)

    print 'The optimum tour is: %s (%f)' % (best_tour["path"], best_tour["distance"])
    print 'There are %d cities in this tour.' % (len(best_tour["path"]))
    print 'This solution took %0.3f seconds to calculate' % (finish - start)
    print 'The results are stored in the file ' + sys.argv[1] + '.tour'
    rfile.close()
    wfile.close()
