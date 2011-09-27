/**
 * @file RandomVariable.hpp
 * Defines the base class for all random variables.
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

#ifndef RANDOMVARIABLE_HPP_
#define RANDOMVARIABLE_HPP_

#include "RandomNumberEngine.hpp"
#include <iosfwd>
#include <string>

namespace cpprob
{

  /**
   * A %RandomVariable represents the outcome of a statistical experiment.
   * A %RandomVariable has a name assigned referring to the experiment. So
   * two RandomVariables are considered to belong to the same experiment and
   * to have the same value structure. For example, a discrete random variable
   * may take on only values of a certain range. Or a Dirichlet distributed
   * random variable is a vector with elements in the range [0, 1].
   *
   * This is the base class for all RandomVariables. It just defines the
   * interface. This interface is common for discrete and continuous
   * one-dimensional and multi-dimensional random variables.
   */
  class RandomVariable
  {

  public:

    /**
     * The virtual desctructor allow polymorphic deletion. It is defined as pure
     * here and as line below. This is the recommended way for pure virtual
     * destructors.
     */
    virtual
    ~RandomVariable() = 0;

    /**
     * Generates a random value in the range of the random variable and
     * assigns it to itself. This may be useful for certain algorithms, e.g.
     * to generate start values.
     *
     * The argument is a C++ random number engine. You can find those engines
     * in the standard header @em random. They are uniform pseudo-random
     * number generators. From these engine, non-uniform number generators can
     * be derived. Here the engine must be of the type defined in the project
     * header RandomNumberEngine.h. It is best to use the global variable
     * cpprob::random_number_engine. In the future, this argument will
     * be removed.
     *
     * @todo Remove rne argument in favor of the global variable.
     *
     * @param rne the random number engine to use as the source of randomness
     */
    virtual void
    assign_random_value(RandomNumberEngine& rne) = 0;

    /**
     * Provides the name of the experiment this variable is assigned with.
     * The name of a random variable might be something like the @em FirstDice,
     * @em SecondDice, @em Flavor, @em Wrapper etc. These names are mostly used
     * for to generate understandable output.
     *
     * In addition, the name of a random variable refers to a set of possible
     * outcome values. The variable can only take on values of this set. For
     * example, if one variable with the name GreenWrapper is a boolean variable
     * (set of outcomes Ω = {true, false}), then another variable with the
     * same name refers to the same set of outcomes.
     *
     * @return a descriptive name associated with this variable
     */
    virtual const std::string&
    name() const = 0;

    /**
     * Puts out the value of a random variable in the form @em name:value.
     * The exact form depends on the value. For example, random vectors need
     * a more extended syntax for their output.
     *
     * This method allow polymorphic output streaming. So every random
     * variable can be streamed out without knowing its exact type.
     *
     * @param os the output stream to use
     * @param r the random variable the value of which is printed out
     * @return the output stream given as the first argument
     */
    friend std::ostream&
    operator<<(std::ostream& os, const RandomVariable& r)
    {
      return r.put_out(os);
    }

  protected:

    /**
     * Puts out the value of this random variable. This virtual method
     * implements the output operation of
     * operator<<(std::ostream&,const RandomVariable&). Child classes
     * override this method to realise the output streaming. They put out their
     * name and value and return the stream that has been passed.
     *
     * @param os the stream to pass the value to
     * @return the streamed received as the argument
     */
    virtual std::ostream&
    put_out(std::ostream& os) const = 0;

  };

  inline
  RandomVariable::~RandomVariable()
  {
  }

}

#endif /* RANDOMVARIABLE_HPP_ */
