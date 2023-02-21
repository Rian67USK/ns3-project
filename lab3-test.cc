#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pcap-file-wrapper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TwoNodeTopology");

int main (int argc, char *argv[])
{
  // Create nodes
  NodeContainer nodes;
  nodes.Create (8); // 2 PTP nodes with 3 nodes each

  // Create point-to-point links
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;

  // Link PTP node 1 with its routing nodes
  devices = p2p.Install (nodes.Get (0), nodes.Get (1));
  devices.Add (p2p.Install (nodes.Get (1), nodes.Get (2)));
  devices.Add (p2p.Install (nodes.Get (1), nodes.Get (3)));

  // Link PTP node 2 with its routing nodes
  devices.Add (p2p.Install (nodes.Get (4), nodes.Get (5)));
  devices.Add (p2p.Install (nodes.Get (4), nodes.Get (6)));
  devices.Add (p2p.Install (nodes.Get (4), nodes.Get (7)));

  // Install Internet stack
  InternetStackHelper stack;
  stack.Install (nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Enable pcap output for all devices
  PcapHelper pcapHelper;
  pcapHelper.EnablePcapAll ("two-node-topology");

  // Print routing table of each node
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  for (uint32_t i = 0; i < nodes.GetN (); i++)
    {
      Ptr<Ipv4> ipv4 = nodes.Get (i)->GetObject<Ipv4> ();
      NS_LOG_INFO ("Routing table for node " << i << ":\n" << ipv4->GetRoutingTable ());
    }

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
