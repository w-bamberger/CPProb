#  Copyright Joel de Guzman 2002-2007. Distributed under the Boost
#  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
#  or copy at http://www.boost.org/LICENSE_1_0.txt)
#  Hello World Example from the tutorial

from proby import *
import csv

print greet()

alpha = 5.0

bn = BayesianNetwork()
params_table = dict()

bag = RandomBoolean("Bag", True)
bag_params = RandomProbabilities(bag)
bag_params_node = bn.add_dirichlet(bag_params, alpha)
flavor = RandomBoolean("Flavor", True)
flavor_params = RandomConditionalProbabilities(flavor, bag)
params_table["Flavor"] = bn.add_conditional_dirichlet(flavor_params, alpha)
hole = RandomBoolean("Hole", True)
hole_params = RandomConditionalProbabilities(hole, bag)
params_table["Hole"] = bn.add_conditional_dirichlet(hole_params, alpha)
wrapper = RandomBoolean("Wrapper", True)
wrapper_params = RandomConditionalProbabilities(wrapper, bag)
params_table["Wrapper"] = bn.add_conditional_dirichlet(wrapper_params, alpha)

csv_reader = csv.DictReader(open("latent-bag.csv"))

for line in csv_reader:
    bag_node = bn.add_categorical(bag, bag_params_node)
    
    for k, v in line.iteritems():
        if k != "Bag":
            var = RandomBoolean(k, v)
            node = bn.add_conditional_categorical(var, [bag_node], params_table[k])
            node.is_evidence = True

print "Extend the network for prediction"

new_bag_node = bn.add_categorical(bag, bag_params_node)
new_flavor_node = bn.add_conditional_categorical(flavor, [new_bag_node], params_table["Flavor"])
new_hole_node = bn.add_conditional_categorical(hole, [new_bag_node], params_table["Hole"])
new_hole_node.is_evidence = True
new_wrapper_node = bn.add_conditional_categorical(wrapper, [new_bag_node], params_table["Wrapper"])
new_wrapper_node.is_evidence = True

print str(bn)

print str(bn.sample(new_flavor_node, 5, 500))
