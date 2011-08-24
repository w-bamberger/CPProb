#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import csv
import random
import sys

records = []

def gen_russel_norvig_records():
    for i in range(273):
        bag = str(random.random() <= 0.05).lower()
    records.append({'Bag':bag, 'Flavor':'true', 'Wrapper':'true', 'Hole':'true'})

    for i in range(93):
        bag = str(random.random() <= 0.33).lower()
        records.append({'Bag':bag, 'Flavor':'true', 'Wrapper':'true', 'Hole':'false'})

    for i in range(104):
        bag = str(random.random() <= 0.33).lower()
        records.append({'Bag':bag, 'Flavor':'true', 'Wrapper':'false', 'Hole':'true'})

    for i in range(90):
        bag = str(random.random() <= 0.821).lower()
        records.append({'Bag':bag, 'Flavor':'true', 'Wrapper':'false', 'Hole':'false'})

    for i in range(79):
        bag = str(random.random() <= 0.33).lower()
        records.append({'Bag':bag, 'Flavor':'false', 'Wrapper':'true', 'Hole':'true'})

    for i in range(100):
        bag = str(random.random() <= 0.821).lower()
        records.append({'Bag':bag, 'Flavor':'false', 'Wrapper':'true', 'Hole':'false'})

    for i in range(94):
        bag = str(random.random() <= 0.821).lower()
        records.append({'Bag':bag, 'Flavor':'false', 'Wrapper':'false', 'Hole':'true'})

    for i in range(167):
        bag = str(random.random() <= 0.977).lower()
        records.append({'Bag':bag, 'Flavor':'false', 'Wrapper':'false', 'Hole':'false'})

    
def gen_custom_records():
    p_bag = 0.2
    p_flavor = [0.3, 0.4]
    p_wrapper = [0.1, 0.7]
    p_hole = [0.6, 0.2]
    
    for r_number in range(50000):
        if random.random() <= p_bag:
            bag = 'true'
            bag_index = 1
        else:
            bag = 'false'
            bag_index = 0
        flavor = str(random.random() <= p_flavor[bag_index]).lower()
        wrapper = str(random.random() <= p_wrapper[bag_index]).lower()
        hole = str(random.random() <= p_hole[bag_index]).lower()
        records.append({'Bag':bag, 'Flavor':flavor, 'Wrapper':wrapper, 'Hole':hole})
    

arg_parser = argparse.ArgumentParser(description='Generates data for the bag network example of Russel and Norvig (2003). The probabilities of the data can be configured. The data is written in a CSV file.')
arg_parser.add_argument('CONFIGURATION', 
                        help='The set of statistics to be used for the data generation. Valid values are "russel-norvig" and "custom".')
arg_parser.add_argument('DATA_FILE', 
                        help='The name of the file in which the generated data is written. The output format are comma separated values.')
program_arguments = arg_parser.parse_args()

if program_arguments.CONFIGURATION == 'russel-norvig':
    gen_russel_norvig_records()
else:
    gen_custom_records()
random.shuffle(records)

csv_file = open(program_arguments.DATA_FILE, 'wb')
header = ('Bag', 'Flavor', 'Wrapper', 'Hole')
csv_file.write(','.join(header) + '\n')
writer = csv.DictWriter(csv_file, header)
writer.writerows(records)
