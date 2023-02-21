/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma1 = 3;
    uint32_t nCsma2 = 3;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma1", "Number of \"extra\" CSMA nodes/devices", nCsma1);
    cmd.AddValue("nCsma2", "Number of \"extra\" CSMA nodes/devices", nCsma2);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma1 = nCsma1 == 0 ? 1 : nCsma1;
    nCsma2 = nCsma2 == 0 ? 1 : nCsma2;

    NodeContainer csma1Nodes;
    NodeContainer csma2Nodes;
    NodeContainer p2pNodes;

    csma1Nodes.Create(nCsma1);
    csma2Nodes.Create(nCsma2);
    p2pNodes.Create(2);

    CsmaHelper csma1;
    csma1.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma1.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    csma1Nodes.Add(p2pNodes.Get(1));
    NetDeviceContainer csma1Devices;
    csma1Devices = csma1.Install(csma1Nodes);

    CsmaHelper csma2;
    csma2.SetChannelAttribute("DataRate", StringValue("1000Mbps"));
    csma2.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    csma2Nodes.Add(p2pNodes.Get(0));
    NetDeviceContainer csma2Devices;
    csma2Devices = csma2.Install(csma2Nodes);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    
    InternetStackHelper stack;
    //stack.Install(p2pNodes.Get(0));
    stack.Install(csma1Nodes);
    //stack.Install(p2pNodes.Get(1));
    stack.Install(csma2Nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.2.0", "255.255.255.0"); //csma1
    Ipv4InterfaceContainer csma2Interfaces;
    csma2Interfaces = address.Assign(csma1Devices); //csma1Devices

    address.SetBase("192.168.12.0", "255.255.255.0"); //csma2
    Ipv4InterfaceContainer csma1Interfaces;
    csma1Interfaces = address.Assign(csma2Devices);

    address.SetBase("10.1.1.0", "255.255.255.0"); //p2p
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    UdpEchoServerHelper echoServer(12);
    ApplicationContainer serverApps = echoServer.Install(csma1Nodes.Get(nCsma1)); //.Get(nCsma1)
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    UdpEchoClientHelper echoClient(csma2Interfaces.GetAddress(nCsma1), 12);
    echoClient.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1012));

    NodeContainer clientNodes (csma1Nodes.Get(0), csma1Nodes.Get(1));

    ApplicationContainer clientApps = echoClient.Install(clientNodes); //p2pNodes.Get(0)
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("second");
    csma1.EnablePcap("second", csma1Devices.Get(1), true);
    csma2.EnablePcap("second", csma2Devices.Get(1), true);


    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
