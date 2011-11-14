/**
 * @file DiscreteNode.hpp
 * Base class for all nodes that have discrete random variables as values.
 *
 * @author Walter Bamberger
 *
 * @par License
 * Copyright (C) 2011 Walter Bamberger
 * @par
 * This file is part of CPProb.
 * @par
 * CPProb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version, with the
 * exceptions mentioned in the file LICENSE.txt.
 * @par
 * CPProb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public
 * License along with CPProb.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef DISCRETENODE_HPP_
#define DISCRETENODE_HPP_

#include "DiscreteRandomVariable.hpp"
#include "cont/list.hpp"
#include <boost/range/adaptor/indirected.hpp>

namespace cpprob
{

  class ConditionalCategoricalNode;

  /*
   *
   */
  class DiscreteNode
  {

  protected:

    typedef cont::list<ConditionalCategoricalNode*> Children;

  public:

    typedef boost::indirected_range<const Children> ConstChildRange;
    typedef boost::indirected_range<Children> ChildRange;

    DiscreteNode(const DiscreteRandomVariable& value)
        : value_(value)
    {
    }

    virtual
    ~DiscreteNode()
    {
    }

    void
    add_child(ConditionalCategoricalNode& child)
    {
      children_.push_back(&child);
    }

    ChildRange
    children();

    ConstChildRange
    children() const;

    DiscreteRandomVariable&
    value()
    {
      return value_;
    }

    const DiscreteRandomVariable&
    value() const
    {
      return value_;
    }

  protected:

    Children children_;
    DiscreteRandomVariable value_;

  };

} /* namespace cpprob */

#endif /* DISCRETENODE_HPP_ */
