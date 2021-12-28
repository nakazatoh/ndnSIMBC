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

#include "bct.hpp"
#include "pit-entry.hpp"
#include "measurements-entry.hpp"

#include <ndn-cxx/util/concepts.hpp>

namespace nfd {
namespace bct {

NDN_CXX_ASSERT_FORWARD_ITERATOR(Bct::const_iterator);

const unique_ptr<Entry> Bct::s_emptyEntry = make_unique<Entry>(Name());

static inline bool
nteHasBctEntry(const name_tree::Entry& nte)
{
  return nte.getBctEntry() != nullptr;
}

Bct::Bct(NameTree& nameTree)
  : m_nameTree(nameTree)
{
}

template<typename K>
const Entry&
Bct::findLongestPrefixMatchImpl(const K& key) const
{
  name_tree::Entry* nte = m_nameTree.findLongestPrefixMatch(key, &nteHasBctEntry);
  if (nte != nullptr) {
    return *nte->getBctEntry();
  }
  return *s_emptyEntry;
}

const Entry&
Bct::findLongestPrefixMatch(const Name& prefix) const
{
  return this->findLongestPrefixMatchImpl(prefix);
}

const Entry&
Bct::findLongestPrefixMatch(const pit::Entry& pitEntry) const
{
  return this->findLongestPrefixMatchImpl(pitEntry);
}

const Entry&
Bct::findLongestPrefixMatch(const measurements::Entry& measurementsEntry) const
{
  return this->findLongestPrefixMatchImpl(measurementsEntry);
}

Entry*
Bct::findExactMatch(const Name& prefix)
{
  name_tree::Entry* nte = m_nameTree.findExactMatch(prefix);
  if (nte != nullptr)
    return nte->getBctEntry();

  return nullptr;
}

std::pair<Entry*, bool>
Bct::insert(const Name& prefix)
{
  name_tree::Entry& nte = m_nameTree.lookup(prefix);
  Entry* entry = nte.getBctEntry();
  if (entry != nullptr) {
    return {entry, false};
  }

  nte.setBctEntry(make_unique<Entry>(prefix));
  ++m_nItems;
  return {nte.getBctEntry(), true};
}

void
Bct::erase(name_tree::Entry* nte, bool canDeleteNte)
{
  BOOST_ASSERT(nte != nullptr);

  nte->setBctEntry(nullptr);
  if (canDeleteNte) {
    m_nameTree.eraseIfEmpty(nte);
  }
  --m_nItems;
}

void
Bct::erase(const Name& prefix)
{
  name_tree::Entry* nte = m_nameTree.findExactMatch(prefix);
  if (nte != nullptr) {
    this->erase(nte);
  }
}

void
Bct::erase(const Entry& entry)
{
  name_tree::Entry* nte = m_nameTree.getEntry(entry);
  if (nte == nullptr) { // don't try to erase s_emptyEntry
    BOOST_ASSERT(&entry == s_emptyEntry.get());
    return;
  }
  this->erase(nte);
}

void
Bct::addOrUpdateNextHop(Entry& entry, Face& face, uint64_t cost)
{
  NextHopList::iterator it;
  bool isNew;
  std::tie(it, isNew) = entry.addOrUpdateNextHop(face, cost);

  if (isNew)
    this->afterNewNextHop(entry.getPrefix(), *it);
}

Bct::RemoveNextHopResult
Bct::removeNextHop(Entry& entry, const Face& face)
{
  bool isRemoved = entry.removeNextHop(face);

  if (!isRemoved) {
    return RemoveNextHopResult::NO_SUCH_NEXTHOP;
  }
  else if (!entry.hasNextHops()) {
    name_tree::Entry* nte = m_nameTree.getEntry(entry);
    this->erase(nte, false);
    return RemoveNextHopResult::BCT_ENTRY_REMOVED;
  }
  else {
    return RemoveNextHopResult::NEXTHOP_REMOVED;
  }
}

Bct::Range
Bct::getRange() const
{
  return m_nameTree.fullEnumerate(&nteHasBctEntry) |
         boost::adaptors::transformed(name_tree::GetTableEntry<Entry>(&name_tree::Entry::getBctEntry));
}

} // namespace fib
} // namespace nfd
