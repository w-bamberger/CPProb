/*
 * random_probability.cpp
 *
 *  Created on: 08.07.2011
 *      Author: wbam
 */

#include "RandomConditionalProbabilities.hpp"
#include "IoUtils.hpp"
#include <tr1/random>
#include <tr1/tuple>
#include <algorithm>

using namespace std;
using namespace std::tr1;

namespace cpprob
{

  RandomConditionalProbabilities::RandomConditionalProbabilities(
      const DiscreteRandomVariable& var,
      const DiscreteRandomVariable& condition) :
      name_("Probabilities" + var.name() + condition.name())
  {
    DiscreteRandomVariable::Range condition_range = condition.value_range();
    if (condition_range.empty())
      {
        cpt_.insert(make_pair(condition, RandomProbabilities(var)));
      }
    else
      {
        for (DiscreteRandomVariable c = condition_range.begin();
            c != condition_range.end(); ++c)
              {
          cpt_.insert(make_pair(c, RandomProbabilities(var)));
        }
    }
}

RandomConditionalProbabilities::~RandomConditionalProbabilities()
{
}

void
RandomConditionalProbabilities::assign_random_value(RandomNumberEngine& rne)
{
  for (iterator i = cpt_.begin(); i != cpt_.end(); ++i)
    i->second.assign_random_value(rne);
}

float
RandomConditionalProbabilities::at(const DiscreteRandomVariable& var,
    const DiscreteRandomVariable& condition) const
{
  ConditionalProbabilityTable::const_iterator found = cpt_.find(condition);
  if (found != cpt_.end())
    return found->second.at(var);
  else
    return 0.0;
}

std::ostream&
RandomConditionalProbabilities::put_out(std::ostream& os) const
{
  os << name_ << ":";
  for_each(cpt_.begin(), cpt_.end(), StreamOut(os, "\n      (", ")"));
  return os;
}

void
RandomConditionalProbabilities::set(const DiscreteRandomVariable& var,
    const DiscreteRandomVariable& condition, float probability)
{
  iterator find_it = cpt_.find(condition);
  if (find_it == cpt_.end())
    {
      /// @todo This additional if is superfluous, but maybe useful for test code.
      std::pair<iterator, bool> insert_result = cpt_.insert(
          std::make_pair(condition, RandomProbabilities(var)));
      if (insert_result.second)
        insert_result.first->second.set(var, probability);
      else
        throw std::runtime_error(
            "RandomConditionalProbabilities: Could not insert a RandomProbability for a new condition.");
    }
  else
    {
      find_it->second.set(var, probability);
    }
}

}
