#ifndef YGP_RANDOM_H
#define YGP_RANDOM_H

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.

#include <random>

namespace YGP {

/// Returns the (per thread) random number engine, seeded non-deterministically
inline std::mt19937& randomEngine() {
    thread_local std::mt19937 engine{std::random_device{}()};
    return engine;
}

/// Returns a uniformly distributed random number in the range [0, upper)
/// \param upper Upper (exclusive) border of the random number; must not be 0
inline unsigned int randomNumber(unsigned int upper) {
    return std::uniform_int_distribution<unsigned int>(0, upper - 1)(randomEngine());
}

} // namespace YGP

#endif
