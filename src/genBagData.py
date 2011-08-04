#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import csv
import random
import sys

records = []

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

random.shuffle(records)

csvFile = open('bag.csv', 'wb')
header = ('Bag', 'Flavor', 'Wrapper', 'Hole')
csvFile.write(','.join(header) + '\n')
writer = csv.DictWriter(csvFile, header)
writer.writerows(records)
