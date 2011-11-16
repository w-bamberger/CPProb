/**
 * @file DirichletProcessNode.hpp
 * Node that represents the component indicator of a Dirichlet process.
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

#ifndef DIRICHLETPROCESSNODE_HPP_
#define DIRICHLETPROCESSNODE_HPP_

#include "ConditionalCategoricalNode.hpp"
#include "DirichletProcessParameters.hpp"

namespace cpprob
{

  /**
   * Implements the Pólya urn scheme. So it realizes a chain of nodes.
   */
  class DirichletProcessNode : public DiscreteNode
  {

    typedef DirichletProcessParameters::Children Children;

  public:

    DirichletProcessNode(DirichletProcessParameters& parameters);

    ~DirichletProcessNode()
    {
    }

    void
    init_sampling();

    bool
    is_evidence() const
    {
      return false;
    }

    const DirichletProcessParameters&
    parameters() const
    {
      return parameters_;
    }

    DirichletProcessParameters&
    parameters()
    {
      return parameters_;
    }

    void
    sample();

  private:

    friend std::ostream&
    operator<<(std::ostream& os, const DirichletProcessNode& node);

    DirichletProcessParameters& parameters_;

  };

} /* namespace cpprob */

#endif /* DIRICHLETPROCESSNODE_HPP_ */
