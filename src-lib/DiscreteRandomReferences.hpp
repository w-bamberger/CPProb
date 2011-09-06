/**
 * @file DiscreteRandomReferences.hpp
 * Collection of references to discrete random variables.
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

#ifndef DISCRETERANDOMREFERENCES_H_
#define DISCRETERANDOMREFERENCES_H_

#include "DiscreteRandomVariable.hpp"
#include "Utils.hpp"
#include <boost/iterator/indirect_iterator.hpp>

#ifdef VANET_DEBUG_MODE
#include <debug/set>
namespace cpprob
{
  namespace cont
  {
    using __gnu_debug::set;
  }
}
#else
#include <set>
namespace cpprob
{
  namespace cont
  {
    using std::set;
  }
}
#endif

namespace cpprob
{

  class DiscreteRandomReferences
  {

    typedef cont::set<DiscreteRandomVariable*,
        indirect_less<DiscreteRandomVariable> > Variables;

  public:

    typedef boost::indirect_iterator<Variables::const_iterator> const_iterator;
    typedef boost::indirect_iterator<Variables::iterator> iterator;
    typedef Variables::size_type size_type;

    DiscreteRandomReferences()
        : characteristics_(DiscreteRandomVariable::characteristics_table_.end())
    {
    }

    template<class Iterator>
      DiscreteRandomReferences(Iterator begin, Iterator end)
          : characteristics_(
              DiscreteRandomVariable::characteristics_table_.end()), references_(
              begin, end)
      {
      }

    ~DiscreteRandomReferences()
    {
    }

    iterator
    begin()
    {
      return iterator(references_.begin());
    }

    const_iterator
    begin() const
    {
      return const_iterator(references_.begin());
    }

    void
    clear()
    {
      references_.clear();
    }

    size_type
    count(const DiscreteRandomVariable& var) const
    {
      DiscreteRandomVariable* const p =
          &const_cast<DiscreteRandomVariable&>(var);
      return references_.count(p);
    }

    iterator
    end()
    {
      return iterator(references_.end());
    }

    const_iterator
    end() const
    {
      return const_iterator(references_.end());
    }

    std::pair<iterator, bool>
    insert(DiscreteRandomVariable& new_reference)
    {
      characteristics_ = DiscreteRandomVariable::characteristics_table_.end();
      return references_.insert(&new_reference);
    }

    DiscreteRandomVariable
    joint_value() const;

    std::size_t
    size() const
    {
      return references_.size();
    }

  private:

    mutable DiscreteRandomVariable::CharacteristicsTable::iterator characteristics_;
    Variables references_;

    void
    set_up_characteristics() const;

  };

} /* namespace cpprob */

#endif /* DISCRETERANDOMREFERENCES_H_ */
