/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef NFD_DAEMON_TABLE_BCT_HPP
#define NFD_DAEMON_TABLE_BCT_HPP

#include "bct-entry.hpp"
#include "name-tree.hpp"

namespace nfd {

namespace measurements {
class Entry;
} // namespace measurements
namespace pit {
class Entry;
} // namespace pit

namespace bct {

/** \brief Represents the Forwarding Information Base (FIB)
 */
class Bct : noncopyable
{
public:
  explicit
  Bct(NameTree& nameTree);

  size_t
  size() const
  {
    return m_nItems;
  }

public: // lookup
  /** \brief Performs a longest prefix match
   */
  const Entry&
  findLongestPrefixMatch(const Name& prefix) const;

  /** \brief Performs a longest prefix match
   *
   *  This is equivalent to `findLongestPrefixMatch(pitEntry.getName())`
   */
  const Entry&
  findLongestPrefixMatch(const pit::Entry& pitEntry) const;

  /** \brief Performs a longest prefix match
   *
   *  This is equivalent to `findLongestPrefixMatch(measurementsEntry.getName())`
   */
  const Entry&
  findLongestPrefixMatch(const measurements::Entry& measurementsEntry) const;

  /** \brief Performs an exact match lookup
   */
  Entry*
  findExactMatch(const Name& prefix);

public: // mutation
  /** \brief Maximum number of components in a FIB entry prefix.
   */
  static constexpr size_t
  getMaxDepth()
  {
    return NameTree::getMaxDepth();
  }

  /** \brief Find or insert a FIB entry
   *  \param prefix FIB entry name; it must not have more than \c getMaxDepth() components.
   *  \return the entry, and true for new entry or false for existing entry
   */
  std::pair<Entry*, bool>
  insert(const Name& prefix);

  void
  erase(const Name& prefix);

  void
  erase(const Entry& entry);

  /** \brief Add a NextHop record
   *
   *  If a NextHop record for \p face already exists in \p entry, its cost is set to \p cost.
   */
  void
  addOrUpdateNextHop(Entry& entry, Face& face, uint64_t cost);

  enum class RemoveNextHopResult {
    NO_SUCH_NEXTHOP, ///< the nexthop is not found
    NEXTHOP_REMOVED, ///< the nexthop is removed and the fib entry stays
    BCT_ENTRY_REMOVED ///< the nexthop is removed and the fib entry is removed
  };

  /** \brief Remove the NextHop record for \p face from \p entry
   */
  RemoveNextHopResult
  removeNextHop(Entry& entry, const Face& face);

public: // enumeration
  typedef boost::transformed_range<name_tree::GetTableEntry<Entry>, const name_tree::Range> Range;
  typedef boost::range_iterator<Range>::type const_iterator;

  /** \return an iterator to the beginning
   *  \note The iteration order is implementation-defined.
   *  \warning Undefined behavior may occur if a FIB/PIT/Measurements/StrategyChoice entry
   *           is inserted or erased during iteration.
   */
  const_iterator
  begin() const
  {
    return this->getRange().begin();
  }

  /** \return an iterator to the end
   *  \sa begin()
   */
  const_iterator
  end() const
  {
    return this->getRange().end();
  }

public: // signal
  /** \brief signals on Fib entry nexthop creation
   */
  signal::Signal<Fib, Name, NextHop> afterNewNextHop;

private:
  /** \tparam K a parameter acceptable to NameTree::findLongestPrefixMatch
   */
  template<typename K>
  const Entry&
  findLongestPrefixMatchImpl(const K& key) const;

  void
  erase(name_tree::Entry* nte, bool canDeleteNte = true);

  Range
  getRange() const;

private:
  NameTree& m_nameTree;
  size_t m_nItems = 0;

  /** \brief The empty FIB entry.
   *
   *  This entry has no nexthops.
   *  It is returned by findLongestPrefixMatch if nothing is matched.
   */
  static const unique_ptr<Entry> s_emptyEntry;
};

} // namespace fib

using fib::Fib;

} // namespace nfd

#endif // NFD_DAEMON_TABLE_FIB_HPP



