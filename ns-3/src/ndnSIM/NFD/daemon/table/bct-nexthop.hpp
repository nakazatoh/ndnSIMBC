/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2019,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_TABLE_BCT_NEXTHOP_HPP
#define NFD_DAEMON_TABLE_BCT_NEXTHOP_HPP

#include "core/common.hpp"
#include "face/face.hpp"

namespace nfd {
namespace bct {

/** \brief Represents a nexthop record in a BCT entry
 */
class NextHop
{
public:
  explicit
  NextHop(Face& face)
    : m_face(&face)
    , m_timestamp(time::steady_clock::now())
  {
  }

  Face&
  getFace() const
  {
    return *m_face;
  }

  time::steady_clock::TimePoint
  getTimestamp() const
  {
    return m_timestamp;
  }

  void
  setTimestamp(time::steady_clock::TimePoint timestamp)
  {
    m_timestamp = timestamp;
  }

private:
  Face* m_face; // pointer instead of reference so that NextHop is movable
//  uint64_t m_cost = 0;
//  time::steady_clock::TimePoint now = time::steady_clock::now();
  time::steady_clock::TimePoint m_timestamp;
};

} // namespace bct
} // namespace nfd

#endif // NFD_DAEMON_TABLE_BCT_NEXTHOP_HPP
