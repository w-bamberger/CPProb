/**
 * @file CategoricalNode.hpp
 * Node with a random variable that follows a categorical distribution.
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

#ifndef CATEGORICALNODE_HPP_
#define CATEGORICALNODE_HPP_

#include "DiscreteNode.hpp"
#include "RandomProbabilities.hpp"

namespace cpprob
{

  class ConditionalCategoricalNode;

  class CategoricalNode : public DiscreteNode
  {

  public:

    CategoricalNode(const DiscreteRandomVariable& value,
        RandomProbabilities& probabilities);

    virtual
    ~CategoricalNode();

    virtual float
    at_references() const
    {
      return probabilities_.at(value_);
    }

    virtual void
    init_sampling();

    RandomProbabilities&
    probabilities()
    {
      return probabilities_;
    }

    const RandomProbabilities&
    probabilities() const
    {
      return probabilities_;
    }

    virtual void
    sample();

  private:

    RandomProbabilities& probabilities_;

    friend std::ostream&
    operator<<(std::ostream& os, const CategoricalNode& node);

  };

} /* namespace cpprob */

#endif /* CATEGORICALNODE_HPP_ */
