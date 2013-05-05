
import csv
from proby import *
import sys
import time

alpha = 5.0

print "Build the network"
start = time.clock()

bn = BayesianNetwork()
params_table = dict()

bag = RandomBoolean("Bag", True)
bag_params = RandomProbabilities(bag)
bag_params_node = bn.add_dirichlet(bag_params, alpha)
flavor = RandomBoolean("Flavor", True)
flavor_params = RandomConditionalProbabilities(flavor, bag)
params_table["Flavor"] = bn.add_conditional_dirichlet(flavor_params, alpha)
wrapper = RandomBoolean("Wrapper", True)
wrapper_params = RandomConditionalProbabilities(wrapper, bag)
params_table["Wrapper"] = bn.add_conditional_dirichlet(wrapper_params, alpha)
hole = RandomBoolean("Hole", True)
hole_params = RandomConditionalProbabilities(hole, bag)
params_table["Hole"] = bn.add_conditional_dirichlet(hole_params, alpha)

csv_reader = csv.DictReader(open("latent-bag.csv"))

for line in csv_reader:
    bag_node = bn.add_categorical(bag, bag_params_node)
    
    for k, v in line.iteritems():
        if k != "Bag":
            var = RandomBoolean(k, v)
            node = bn.add_conditional_categorical(var, [bag_node], params_table[k])
            node.is_evidence = True

print "Duration:", time.clock() - start

print "Extend the network for prediction"
new_bag_node = bn.add_categorical(bag, bag_params_node)
new_hole_node = bn.add_conditional_categorical(hole, [new_bag_node], params_table["Hole"])
new_hole_node.is_evidence = True
new_wrapper_node = bn.add_conditional_categorical(wrapper, [new_bag_node], params_table["Wrapper"])
new_wrapper_node.is_evidence = True
new_flavor_node = bn.add_conditional_categorical(flavor, [new_bag_node], params_table["Flavor"])

burn_in_iterations = 5
collect_iterations = 500
print "Sample with", burn_in_iterations, "burn-in iterations and", collect_iterations, "collect iterations."
start = time.clock()
prediction = bn.sample(new_flavor_node, burn_in_iterations, collect_iterations)
duration = time.clock() - start
print "Prediction:",
# prediction already ends with a line break
sys.stdout.write(str(prediction))   
print "Duration:", duration

max_deviation = 0.02
iterations = 0
print "Sample with automatic convergence at a maximum deviation of", max_deviation
start = time.clock()
(prediction, iterations) = bn.sample(new_flavor_node, max_deviation)
duration = time.clock() - start
print "Prediction:",
# prediction already ends with a line break
sys.stdout.write(str(prediction))   
print "Duration:", duration
print "Iterations:", iterations

