/**
 * @file ConditionalDirichletNode.hpp
 * Node with several sets of random probabilities each of which
 * is Dirichlet distributed.
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

#ifndef CONDITIONALDIRICHLETNODE_HPP_
#define CONDITIONALDIRICHLETNODE_HPP_

#include "ConditionalCategoricalNode.hpp"

namespace cpprob
{

  class ConditionalDirichletNode
  {

  public:

    typedef cont::RefVector<ConditionalCategoricalNode> Children;
    typedef DiscreteRandomVariableMap<float> Parameters;

    ConditionalDirichletNode(const RandomConditionalProbabilities& value,
        float alpha);

    Children&
    children()
    {
      return children_;
    }

    const Children&
    children() const
    {
      return children_;
    }

    void
    init_sampling();

    Parameters&
    parameters()
    {
      return parameters_;
    }

    const Parameters&
    parameters() const
    {
      return parameters_;
    }

    void
    sample();

    void
    sample(const DiscreteRandomVariable& condition,
        const Children& children);

    RandomConditionalProbabilities&
    value()
    {
      return value_;
    }

    const RandomConditionalProbabilities&
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

  private:

    bool is_evidence_;
    Children children_;
    RandomConditionalProbabilities value_;
    Parameters parameters_;

    friend std::ostream&
    operator<<(std::ostream& os, const ConditionalDirichletNode& node);

  };

} /* namespace cpprob */

#endif /* CONDITIONALDIRICHLETNODE_HPP_ */
