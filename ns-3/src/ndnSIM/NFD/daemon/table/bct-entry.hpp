/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef NFD_DAEMON_TABLE_BCT_ENTRY_HPP
#define NFD_DAEMON_TABLE_BCT_ENTRY_HPP

#include "fib-nexthop.hpp"
#include "ns-3/nstime.h"

namespace nfd {

namespace name_tree {
class Entry;
} // namespace name_tree

namespace bct {

class Bct;

/** \class nfd::fib::NextHopList
 *  \brief Represents a collection of nexthops.
 *
 *  This type has the following member functions:
 *  - `iterator<NextHop> begin()`
 *  - `iterator<NextHop> end()`
 *  - `size_t size()`
 */
using NextHopList = std::vector<NextHop>;

/** \brief represents a BCT entry
 */
class Entry : noncopyable
{
public:
  explicit
  Entry(const Name& prefix);

  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  const NextHopList&
  getNextHops() const
  {
    return m_nextHops;
  }

  /** \return whether this Entry has any NextHop record
   */
  bool
  hasNextHops() const
  {
    return !m_nextHops.empty();
  }

  /** \return whether there is a NextHop record for \p face
   */
  bool
  hasNextHop(const Face& face) const;

private:
  /** \brief adds a NextHop record to the entry
   *
   *  If a NextHop record for \p face already exists in the entry, its cost is set to \p cost.
   *
   *  \return the iterator to the new or updated NextHop and a bool indicating whether a new
   *  NextHop was inserted
   */
  std::pair<NextHopList::iterator, bool>
  addOrUpdateNextHop(Face& face, uint64_t cost);

  /** \brief removes a NextHop record
   *
   *  If no NextHop record for face exists, do nothing.
   */
  bool
  removeNextHop(const Face& face);

  /** \note This method is non-const because mutable iterators are needed by callers.
   */
  NextHopList::iterator
  findNextHop(const Face& face);

  /** \brief sorts the nexthop list
   */
  void
  sortNextHops();

private:
  
  Name m_prefix;
  NextHopList m_nextHops;
  Time m_timestamp;

  name_tree::Entry* m_nameTreeEntry = nullptr;

  friend class name_tree::Entry;
  friend class Cbt;
};

} // namespace cbt
} // namespace nfd

#endif // NFD_DAEMON_TABLE_CS_ENTRY_HPP
