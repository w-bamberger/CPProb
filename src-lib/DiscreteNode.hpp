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

#include "CategoricalDistribution.hpp"
#include <boost/range/adaptor/indirected.hpp>

#ifdef VANET_DEBUG_MODE
#include <debug/list>
namespace cpprob
{
  namespace cont
  {
    using __gnu_debug::list;
  }
}
#else
#include <list>
namespace cpprob
{
  namespace cont
  {
    using std::list;
  }
}
#endif

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
        : is_evidence_(false), value_(value), sampling_variate_(
            random_number_engine, CategoricalDistribution())
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

    virtual float
    at_references() const = 0;

    virtual void
    init_sampling() = 0;

    virtual void
    sample() = 0;

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

    bool
    is_evidence() const
    {
      return is_evidence_;
    }

    void
    is_evidence(bool value_is_evidence)
    {
      is_evidence_ = value_is_evidence;
    }

  protected:

    bool is_evidence_;
    DiscreteRandomVariable value_;
    Children children_;
    std::variate_generator<RandomNumberEngine&, CategoricalDistribution> sampling_variate_;

  };

} /* namespace cpprob */

#endif /* DISCRETENODE_HPP_ */
