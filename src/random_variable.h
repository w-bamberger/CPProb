/**
 * @file random_variable.h
 * Defines the base class for all random variables.
 *
 * @author Walter Bamberger
 *
 * $Revision: 535 $
 * $HeadURL: https://shawn.svn.sourceforge.net/svnroot/shawn/src/apps/vanet/vanet_message.h $
 * $Id: vanet_message.h 535 2011-04-12 00:18:18Z andylz $
 *
 * @par License
 * Copyright (C) 2011 Walter Bamberger<br>
 * All rights reserved.<br>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Walter Bamberger nor the names of the contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 * .
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RANDOM_VARIABLE_H_
#define RANDOM_VARIABLE_H_

#include "random_number_engine.h"
#include <iosfwd>
#include <string>

namespace vanet
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
     * in the standard TR1 header @em random. They are uniform pseudo-random
     * number generators. From these engine, non-uniform number generators can
     * be derived. Here the engine must be of the type defined in the project
     * header random_number_engine.h. It is best to use the global variable
     * vanet::random_number_engine. In the future, this argument will
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
     * Puts ou thte value of this random variable. This virtual method
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

#endif /* RANDOM_VARIABLE_H_ */
