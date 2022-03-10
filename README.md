Note:

This is a custom and unsupported fork of NS-3 simulator (http://www.nsnam.org/).


ndnSIM with breadcrumbs extension
================================

In order to add the routing capability exploiting breadcrumbs, the following source files are added to the original ndnSIM.

* ns-3/src/ndnSIM/NFD/daemon/fw/best-route-breadcrumbs-strategy.hpp
* ns-3/src/ndnSIM/NFD/daemon/fw/best-route-breadcrumbs-strategy.cpp
* ns-3/src/ndnSIM/NFD/daemon/table/bct-entry.hpp
* ns-3/src/ndnSIM/NFD/daemon/table/bct-entry.cpp
* ns-3/src/ndnSIM/NFD/daemon/table/bct-nexthop.hpp
* ns-3/src/ndnSIM/NFD/daemon/table/bct.hpp
* ns-3/src/ndnSIM/NFD/daemon/table/bct.cpp

In addition, the following files are modified.

* ns-3/src/ndnSIM/NFD/daemon/fw/forwarder.cpp
* ns-3/src/ndnSIM/NFD/daemon/fw/algorithm.hpp
* ns-3/src/ndnSIM/NFD/daemon/fw/algorithm.cpp
* ns-3/src/ndnSIM/NFD/daemon/fw/strategy.hpp
* ns-3/src/ndnSIM/NFD/daemon/table/name-tree-entry.hpp
* ns-3/src/ndnSIM/NFD/daemon/table/name-tree-entry.cpp

Furthermore, the follwoing files are created for testing purpose.

* ns-3/src/ndnSIM/examples/ndn-grid-with-bc.cpp
* ns-3/src/ndnSIM/examples/ndn-simple-with-bc.cpp

The breadcrumbs table used for forwarding interest packets remove entries which are more than 10 seconds old.  The value is embedded in the source code in this program.  If you need flexibility in the value to execute multiple simulations, you need to fix the mechanism to change the value dynamically.
