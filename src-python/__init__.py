from _proby import *

def joint(variables):
    joint_variable = DiscreteJointRandomVariable()
    for v in variables:
        joint_variable.add(v)
    return joint_variable
