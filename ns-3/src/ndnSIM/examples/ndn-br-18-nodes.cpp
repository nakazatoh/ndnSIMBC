/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-simple-with-bc-18-nodes-updated.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include <string>
#include <iomanip>
#include <iostream>
#include <time.h>

namespace ns3 {

using ns3::ndn::StackHelper;
using ns3::ndn::AppHelper;
using ns3::ndn::GlobalRoutingHelper;
using ns3::ndn::StrategyChoiceHelper;

class PcapWriter {
public:
  PcapWriter(const std::string& file)
  {
    PcapHelper helper;
    m_pcap = helper.CreateFile(file, std::ios::out, PcapHelper::DLT_PPP);
  }

void
TracePacket(Ptr<const Packet> packet)
{
  static PppHeader pppHeader;
  pppHeader.SetProtocol(0x0077);

  m_pcap->Write(Simulator::Now(), pppHeader, packet);
}

private:
  Ptr<PcapFileWrapper> m_pcap;
};

/**
 * This scenario simulates a very simple network topology:
 *
 *
 *      +----------+     1Mbps      +--------+     1Mbps      +----------+
 *      | consumer | <------------> | router | <------------> | producer |
 *      +----------+         10ms   +--------+          10ms  +----------+
 *
 *
 * Consumer requests data from producer with frequency 10 interests per second
 * (interests contain constantly increasing sequence number).
 *
 * For every received interest, producer replies with a data packet, containing
 * 1024 bytes of virtual payload.
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-simple
 */

int
main(int argc, char* argv[])
{


  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(18);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install(nodes.Get(1), nodes.Get(2));
  p2p.Install(nodes.Get(2), nodes.Get(9));
  p2p.Install(nodes.Get(2), nodes.Get(11));
  p2p.Install(nodes.Get(2), nodes.Get(3));
  p2p.Install(nodes.Get(1), nodes.Get(12));
  p2p.Install(nodes.Get(12), nodes.Get(4));
  p2p.Install(nodes.Get(3), nodes.Get(10));
  p2p.Install(nodes.Get(3), nodes.Get(4));
  p2p.Install(nodes.Get(10), nodes.Get(11));
  p2p.Install(nodes.Get(10), nodes.Get(14));
  p2p.Install(nodes.Get(5), nodes.Get(14));
  p2p.Install(nodes.Get(14), nodes.Get(15));
  p2p.Install(nodes.Get(15), nodes.Get(8));
  p2p.Install(nodes.Get(8), nodes.Get(16));
  p2p.Install(nodes.Get(16), nodes.Get(17));
  p2p.Install(nodes.Get(4), nodes.Get(5));
  p2p.Install(nodes.Get(5), nodes.Get(7));
  p2p.Install(nodes.Get(7), nodes.Get(16));
  p2p.Install(nodes.Get(0), nodes.Get(12));
  p2p.Install(nodes.Get(12), nodes.Get(6));
  p2p.Install(nodes.Get(6), nodes.Get(13));
  p2p.Install(nodes.Get(5), nodes.Get(6));
  p2p.Install(nodes.Get(6), nodes.Get(16));

  NodeContainer routerNodes;
  NodeContainer cacheConsumerNodes;
  NodeContainer consumerNodes;

  routerNodes.Add(nodes.Get(1));
  routerNodes.Add(nodes.Get(2));
  routerNodes.Add(nodes.Get(3));
  routerNodes.Add(nodes.Get(4));
  routerNodes.Add(nodes.Get(5));
  routerNodes.Add(nodes.Get(6));
  routerNodes.Add(nodes.Get(7));
  routerNodes.Add(nodes.Get(8));
  routerNodes.Add(nodes.Get(10));
  routerNodes.Add(nodes.Get(12));
  routerNodes.Add(nodes.Get(14));
  routerNodes.Add(nodes.Get(16));

  cacheConsumerNodes.Add(nodes.Get(0));
  cacheConsumerNodes.Add(nodes.Get(9));
  cacheConsumerNodes.Add(nodes.Get(11));

  consumerNodes.Add(nodes.Get(13));
  consumerNodes.Add(nodes.Get(15));

  Ptr<Node> producer = nodes.Get(17);

  ndn::StackHelper routerHelper;
  routerHelper.SetDefaultRoutes(true);
  routerHelper.setCsSize(0);
  routerHelper.Install(routerNodes);
  routerHelper.Install(consumerNodes);
  routerHelper.Install(producer);

  ndn::StackHelper cacheRouterHelper;
  cacheRouterHelper.setCsSize(8);
  cacheRouterHelper.Install(cacheConsumerNodes);

  // install global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Installing NDN applications
  std::string prefix = "/waseda.jp/nzlab";
  
  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll(prefix, "/localhost/nfd/strategy/best-route");

  // cacheConsumer
  ndn::AppHelper cacheConsumerHelper("ns3::ndn::ConsumerZipfMandelbrot");
  // Consumer will request /prefix/0, /prefix/1, ...
  cacheConsumerHelper.SetPrefix(prefix);
  cacheConsumerHelper.SetAttribute("Frequency", StringValue("4")); // 10 interests a second
  cacheConsumerHelper.SetAttribute("NumberOfContents", StringValue("10")); // 10 different contents
  ApplicationContainer cacheApps = cacheConsumerHelper.Install(cacheConsumerNodes); // Consumer (For Cache) 
  // ApplicationContainer cacheApps = cacheConsumerHelper.Install(nodes.Get(0));
  // cacheApps.Stop(Seconds(0.6));

  // Consumer
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerZipfMandelbrot");

  // Consumer will request /prefix/0, /prefix/1, ...
  ns3::PacketMetadata::Enable ();
  consumerHelper.SetPrefix(prefix);
  consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
  consumerHelper.SetAttribute("NumberOfContents", StringValue("10")); // 10 different contents
  ApplicationContainer consumerApps = consumerHelper.Install(consumerNodes); 
  // ApplicationContainer consumerApps = consumerHelper.Install(nodes.Get(13)); 
  // consumerApps.Stop(Seconds(0.0));
  // consumerApps.Start(Seconds(0.6));

  // Producer
  ndn::AppHelper producerHelper("ns3::ndn::Producer");

  // Producer will reply to all requests starting with /prefix
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(producer);// Producer

  //Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producer);
 
  //Calculate and install FIBS
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  time_t t = time(NULL);
  const tm* localTime = localtime(&t);
  std::stringstream s;
  s << localTime->tm_year + 1900;
  s << std::setw(2) << std::setfill('0') << localTime->tm_mon + 1;
  s << std::setw(2) << std::setfill('0') << localTime->tm_mday;
  s << std::setw(2) << std::setfill('0') << localTime->tm_hour;
  s << std::setw(2) << std::setfill('0') << localTime->tm_min;
  s << std::setw(2) << std::setfill('0') << localTime->tm_sec;

  std::string pcap_trace("br-trace.pcap");
  std::string app_delay_trace("br-app-delay-trace.txt");
  std::string rate_trace("br-rate-trace.txt");

  PcapWriter trace(s.str() + pcap_trace);
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacTx",
                                MakeCallback(&PcapWriter::TracePacket, &trace));

  ndn::AppDelayTracer::InstallAll(s.str() + app_delay_trace);
  ndn::L3RateTracer::InstallAll(s.str() + rate_trace, Seconds (1.0));


  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{

  return ns3::main(argc, argv);
}
