/**
 * @file map.hpp
 * Includes the standard map header in a configurable mode.
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

#ifndef CPPROB_CONT_MAP_HPP_
#define CPPROB_CONT_MAP_HPP_

#if defined CPPROB_DEBUG_MODE && defined __GNUC__

#include <debug/map>
namespace cpprob
{
  namespace cont
  {
    using __gnu_debug::map;
  }
}

#else

#include <map>
namespace cpprob
{
  namespace cont
  {
    using std::map;
  }
}

#endif

#endif /* CPPROB_CONT_MAP_HPP_ */
