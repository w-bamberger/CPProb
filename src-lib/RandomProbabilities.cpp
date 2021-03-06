/*
 * RandomProbabilities.cpp
 *
 *  Created on: 26.07.2011
 *      Author: wbam
 */

#include "RandomProbabilities.hpp"
#include "IoUtils.hpp"
#include <algorithm>

using namespace std;

namespace cpprob
{

  RandomProbabilities::Names RandomProbabilities::names_;

  RandomProbabilities::RandomProbabilities() :
      name_(names_.insert("").first)
  {
  }

  RandomProbabilities::RandomProbabilities(const DiscreteRandomVariable& var) :
      name_(names_.insert("Probabilities" + var.name()).first)
  {
    DiscreteRandomVariable::Range range = var.value_range();
    if (!range.empty())
    {
      float p = 1.0f / static_cast<float>(range.size());
      for (DiscreteRandomVariable v = range.begin(); v != range.end(); ++v)
        pt_.insert(make_pair(v, p));
    }
  }

  RandomProbabilities::~RandomProbabilities()
  {
  }

  void
  RandomProbabilities::assign_random_value(RandomNumberEngine& rne)
  {
    /// @todo Creating the variate every time from scratch is slow.
    typedef uniform_real<float> Distribution;
    Distribution distribution(0.0f, 1.0f);
    variate_generator<RandomNumberEngine&, Distribution> variate(rne,
        distribution);

    float sum = 0.0;
    for (iterator i = pt_.begin(); i != pt_.end(); ++i)
    {
      i->second = variate();
      sum += i->second;
    }
    for (iterator i = pt_.begin(); i != pt_.end(); ++i)
      i->second /= sum;
  }

  void
  RandomProbabilities::normalize()
  {
    // Accumulate
    float sum = 0.0;
    for (iterator it = pt_.begin(); it != pt_.end(); ++it)
    {
      sum += it->second;
    }
    // Divide
    for (iterator it = pt_.begin(); it != pt_.end(); ++it)
    {
      it->second /= sum;
    }
  }

  std::ostream&
  RandomProbabilities::put_out(std::ostream& os) const
  {
    os << *name_ << ":";
    for_each(pt_.begin(), pt_.end(), StreamOut(os, ",", ""));
    return os;
  }

}
