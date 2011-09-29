/*
 * random_variable.cpp
 *
 *  Created on: 27.05.2011
 *      Author: wbam
 */

#include "DiscreteRandomVariable.hpp"
#include <ostream>

using namespace std;

namespace cpprob
{

  DiscreteRandomVariable::CharacteristicsTable DiscreteRandomVariable::characteristics_table_;
  const string DiscreteRandomVariable::empty_string_;

  void
  DiscreteRandomVariable::assign_random_value(RandomNumberEngine& rng)
  {
    cpprob_check_debug(
        characteristics_ != characteristics_table_.end(),
        "DiscreteRandomVariable: Cannot compute random value for an empty random variable.");

    /// @todo Creating the variate every time from scratch is slow.
    typedef uniform_int<size_t> Distribution;
    Distribution distribution(0, characteristics_->second.size_ - 1);
    variate_generator<RandomNumberEngine&, Distribution> variate(rng,
        distribution);
    value_ = variate();
  }

  bool
  DiscreteRandomVariable::operator<(const DiscreteRandomVariable& var) const
  {
    /*
     * The default value of characteristics, i.e. characteristics_table_.end(),
     * is taken as the highest value. So the ordering of the random variable
     * store objects is in order of the iterators of CharacteristicsTable.
     * The sub-odering criterion, in case both iterators are the same,
     * is the value_.
     */
    if (characteristics_ == characteristics_table_.end())
      /*
       * No element can be larger than characteristics.
       * (value_ cannot be set.)
       */
      return false;

    else if (var.characteristics_ == characteristics_table_.end())
      /*
       * characteristics must be different and thus smaller
       * than var.characteristics.
       */
      return true;

    else if (characteristics_ == var.characteristics_)
      return value_ < var.value_;

    else
      return characteristics_->first < var.characteristics_->first;
  }

  DiscreteRandomVariable&
  DiscreteRandomVariable::operator=(const DiscreteRandomVariable& var)
  {
    cpprob_check_debug(var.characteristics_ != characteristics_table_.end(),
        "DiscreteRandomVariable: Cannot assign from empty random variable.");
    cpprob_check_debug(
        characteristics_ == characteristics_table_.end() || characteristics_ == var.characteristics_,
        "DiscreteRandomVariable: Cannot assign random variable " //
        + var.characteristics_->first + " to random variable "//
        + characteristics_->first);

    characteristics_ = var.characteristics_;
    value_ = var.value_;
    return *this;
  }

  ostream&
  DiscreteRandomVariable::put_out(ostream& os) const
  {
    if (characteristics_ != characteristics_table_.end())
      return os << characteristics_->first << ":" << value_;
    else
      return os << "(empty random variable)";
  }

  ostream&
  DiscreteRandomVariable::put_out_characteristics_table(ostream& os) const
  {
    os << "Characteristics table:\n";
    for (CharacteristicsTable::iterator it = characteristics_table_.begin();
        it != characteristics_table_.end(); ++it)
    {
      os << it->first << ": " << it->second.size_ << "\n";
    }
    return os;
  }

  //----------------------------------------------------------------------------

  void
  DiscreteRandomVariable::set_up_characteristics(const string& name,
      size_t size)
  {
    characteristics_ = characteristics_table_.lower_bound(name);
    if (characteristics_ == characteristics_table_.end()
        || characteristics_->first != name)
    {
      if (characteristics_ == characteristics_table_.begin())
        characteristics_ = characteristics_table_.insert(
            make_pair(name, Characteristics(size))).first;
      else
        characteristics_ = characteristics_table_.insert(--characteristics_,
            make_pair(name, Characteristics(size)));
    }
  }

}
