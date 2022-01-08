/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bct-entry.hpp"
#include "ns3/simulator.h"

namespace nfd {
namespace bct {

Entry::Entry(const Name& prefix)
  : m_prefix(prefix)
{
}

NextHopList::iterator
Entry::findNextHop(const Face& face)
{
  return std::find_if(m_nextHops.begin(), m_nextHops.end(),
                      [&face] (const NextHop& nexthop) {
                        return &nexthop.getFace() == &face;
                      });
}

bool
Entry::hasNextHop(const Face& face) const
{
  return const_cast<Entry*>(this)->findNextHop(face) != m_nextHops.end();
}

std::pair<NextHopList::iterator, bool>
Entry::addOrUpdateNextHop(Face& face, time::steady_clock::TimePoint timestamp)
{
  auto it = this->findNextHop(face);
  bool isNew = false;
  if (it == m_nextHops.end()) {
    m_nextHops.emplace_back(face);
    it = std::prev(m_nextHops.end());
    isNew = true;
  }

  it->setTimestamp(timestamp);
  this->sortNextHops();

  return std::make_pair(it, isNew);
}

bool
Entry::removeNextHop(const Face& face)
{
  auto it = this->findNextHop(face);
  if (it != m_nextHops.end()) {
    m_nextHops.erase(it);
    return true;
  }
  return false;
}

void
Entry::sortNextHops()
{
  std::sort(m_nextHops.begin(), m_nextHops.end(),
            [] (const NextHop& a, const NextHop& b) { return a.getTimestamp() > b.getTimestamp(); });
}

void
Entry::removeTardyNextHops(time::steady_clock::TimePoint th_time)
{
  auto result = std::find_if(m_nextHops.begin(), m_nextHops.end(), 
            [=](const NextHop& x){ return x.getTimestamp() < th_time; });
  for (; result != m_nextHops.end(); result++) {
    m_nextHops.erase(result);
  }
}

} // namespace bct
} // namespace nfd
