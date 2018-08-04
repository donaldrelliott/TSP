import sys 
import time
import math 
import random
from operator import add


class ACO:
    def __init__(self, alpha, beta, scent, evap_rate):
        self.not_visited = range(0, num_cities)
        self.distance_travelled = 0
        self.route = []
        self.evap_rate = evap_rate
        self.scent = scent
        self.alpha = alpha
        self.beta = beta
        self.tours = 0
        self.current = num_cities

    def getNextTown(self, current):
        nexttown = [[] for i in xrange(num_cities)]
        for town in xrange(0, num_cities):
            if (town not in self.not_visited or town == current):
                nexttown[town] = -1
                continue
            if distance[current][town] == 0:
                pass
            else:
                nexttown[town] = (math.pow(tau[current][town], self.alpha) + math.pow(1 / distance[current][town], self.beta))        
        
        a = nexttown.index(max(nexttown))
        return a

    def iteration(self):
        self.distance_travelled = 0
        self.route = []
        self.route.append(self.current)
        self.not_visited.pop((self.current))
        while(self.not_visited):
            nextt = self.getNextTown(self.current)
            self.distance_travelled += distance[self.current][nextt]
            self.route.append(nextt)
            self.not_visited.pop(self.not_visited.index(nextt))
            self.current = nextt
        if(not self.not_visited):
            self.tourEnded()

    def tourEnded(self):
        self.updateTau()
        self.not_visited = range(0, num_cities)
        self.tours = self.tours + 1
        self.distance_travelled += distance[self.route[-1]][self.route[0]]
        self.current = random.randint(0,num_cities-1)

    def updateTau(self):
        for i in xrange(0, len(self.route) - 1):
            tau[self.route[i]][self.route[i + 1]] = (float(
                tau[self.route[i]][self.route[i + 1]]) * self.evap_rate) + (self.scent / self.distance_travelled)
            tau[self.route[i + 1]][self.route[i]] = (float(
                tau[self.route[i + 1]][self.route[i]]) * self.evap_rate) + (self.scent / self.distance_travelled)

    def place_ant(self):
        rand = random.randint(0,(num_cities-1))
        self.current = rand % num_cities
        
def parseInput():
    cities = list()
    route = dict()    
    input = sys.argv[1]
    with open(input, 'r') as fin:
        lines = fin.readlines()
        for line in lines:
            line = line.split()
            line = [int(line[0]), int(line[2]), int(line[1])]
            route[int(line[0])] = [int(line[2]), int(line[1])]
            cities.append(line)
    return cities, route
    
def init_distances(node_list, distances):
    i=0
    for a in node_list:
        for b in node_list:
            dist = round(math.sqrt((a[1] - b[1])**2 + (a[2] - b[2])**2))
            distances[int(a[0])][int(b[0])] = dist 
    return (distances)
    
def find_distance(a, b):
    dx = a[1] - b[1]
    dy = a[2] - b[2]
    result = int(round(math.sqrt(dx * dx + dy * dy)))
    return result
    
def repeated_nearest(cities):
    size = len(cities)
    if size >= 5000:
        iterations = 3
    else:
        iterations = size
    min_dist = float('inf')
    for i in range(0, iterations):
        unvisited = cities[:]
        if size >=5000: 
            x = random.randint(0,(len(cities)-1))
        else: x = i
        start_city = cities[x]
        path = [start_city]
        unvisited.remove(start_city)
        path_distance = 0
        while unvisited:
            min_distance = float('inf')
            for city in unvisited:
                distance = find_distance(path[-1], city)
                if min_distance > distance:
                    nearest_city = city
                    min_distance = distance
            path_distance += min_distance
            unvisited.remove(nearest_city)
            path.append(nearest_city)
        distance = find_distance(nearest_city, start_city)
        path_distance += distance
        if path_distance < min_dist:
            min_dist = path_distance
            min_path = list()
            for p in path:
                min_path.append(p[0])
    return min_path, min_dist

#GLOBALS
cities, route  = parseInput()
num_cities = len(cities)
distance = [[0 for i in xrange(num_cities)] for i in xrange(num_cities)]
tau = [[0 for i in xrange(num_cities)] for i in xrange(num_cities)]
    
if __name__ == "__main__":
    start = time.time()
    # For Graphe with over 5,000 vertices use the repeated nearest neighbor algorithm
    if num_cities > 500:
        min_path, min_dist = repeated_nearest(cities)
        end = time.time()
        total = end-start
        
        print "-------------------------------------------------------------------------"
        print 'Time: ' + str(total) + 's'
        print 'Route: ' + str(min_path)
        print 'Distance: ' + str(min_dist)
        print "-------------------------------------------------------------------------"  
        
        out = sys.argv[1] + '.tour'
        with open(out, 'w') as f:
            f.write(str(int(min_dist))+'\n')
            for v in min_path:
                f.write(str(v) + '\n')
            
        sys.exit(0)
        
    # For all other graphs use ACO Algorithm
    else:
        distance = init_distances(cities,distance)
        
        # Paramters for ACO:
        num_ants = 5
        evap_rate =0.01
        scent = 0.1
        alpha = 1
        beta = 0.8

        colony = [ACO(alpha, beta, scent, evap_rate)
                  for i in xrange(0, num_ants)]
        tours = 5
        
        for ant in colony:
            ant.place_ant()

        min_route = []
        min_dist = float('inf')
        
        for k in xrange(0, 3):
            for i in xrange(0, tours):
                for ant in colony:
                    ant.iteration()
                    if (ant.distance_travelled) < min_dist:
                        min_dist = (ant.distance_travelled)
                        min_route = (ant.route)
        end = time.time()
        total = end-start
        
        print "-------------------------------------------------------------------------"
        print 'Time: ' + str(total) + 's'
        print 'Route: ' + str(min_route)
        print 'Distance: ' + str(min_dist)
        print "-------------------------------------------------------------------------"
        
        out = sys.argv[1] + '.tour'
        with open(out, 'w') as f:
            f.write(str(int(min_dist))+'\n')
            for v in min_route:
                f.write(str(v) + '\n')
        sys.exit(0)
