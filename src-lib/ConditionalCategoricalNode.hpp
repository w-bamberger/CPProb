/**
 * @file ConditionalCategoricalNode.hpp
 * Node with a random variable that follows categorical distributions
 * that depend on other discrete variables.
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

#ifndef CONDITIONALCATEGORICALNODE_H_
#define CONDITIONALCATEGORICALNODE_H_

#include "DiscreteNode.hpp"
#include "DiscreteRandomReferences.hpp"
#include "RandomConditionalProbabilities.hpp"

namespace cpprob
{

  class ConditionalCategoricalNode : public DiscreteNode
  {

  public:

    ConditionalCategoricalNode(const DiscreteRandomVariable& value,
        const DiscreteRandomReferences& condition,
        RandomConditionalProbabilities& cpt);

    virtual
    ~ConditionalCategoricalNode();

    virtual float
    at_references() const
    {
      return probabilities_.at(condition_.joint_value()).at(value_);
    }

    const DiscreteRandomReferences&
    condition() const
    {
      return condition_;
    }

    virtual void
    init_sampling();

    RandomConditionalProbabilities&
    probabilities()
    {
      return probabilities_;
    }

    const RandomConditionalProbabilities&
    probabilities() const
    {
      return probabilities_;
    }

    virtual void
    sample();

  private:

    DiscreteRandomReferences condition_;
    RandomConditionalProbabilities& probabilities_;

    friend std::ostream&
    operator<<(std::ostream& os, const ConditionalCategoricalNode& node);

  };

} /* namespace cpprob */

#endif /* CONDITIONALCATEGORICALNODE_H_ */
