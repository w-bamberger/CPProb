/*
 * DiscreteNode.cpp
 *
 *  Created on: 05.09.2011
 *      Author: wbam
 */

#include "DiscreteNode.hpp"
#include "ConditionalCategoricalNode.hpp"

namespace cpprob
{

  DiscreteNode::ChildRange
  DiscreteNode::children()
  {
    return boost::adaptors::indirect(children_);
  }

  DiscreteNode::ConstChildRange
  DiscreteNode::children() const
  {
    return boost::adaptors::indirect(children_);
  }

} /* namespace cpprob */
