#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"


using namespace ns3;

// NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char *argv[])
{
    CommandLine cmd;

    // bool verbose = true; // verbose bby
    uint32_t n1 = 4;
    uint32_t n2 = 4;

    
    cmd.AddValue("n1", "Number of CSMA 1 nodes", n1);
    cmd.AddValue("n2", "Number of CSMA 2 nodes", n2);

    cmd.Parse (argc, argv);

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // csmanodes1
    NodeContainer csmaNodes1;
    
    // csmanodes2
    NodeContainer csmaNodes2;
    
    // ptp
    NodeContainer p2pNodes;
   
    // create node1-2 dan ptp
   csmaNodes1.Create (n1);
   csmaNodes2.Create (n2);
   p2pNodes.Create (2);

    CsmaHelper csma1;
    csma1.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma1.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    // add csma 1 ke ptpnodes
    csmaNodes1.Add(p2pNodes.Get(0));
    NetDeviceContainer csma1Devices;
    csma1Devices = csma1.Install(csmaNodes1); // install csmanode 1

    CsmaHelper csma2;
    csma2.SetChannelAttribute("DataRate", StringValue("1000Mbps"));
    csma2.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // add csma 2 ke ptpnodes
    csmaNodes2.Add(p2pNodes.Get(1));
    NetDeviceContainer csma2Devices;
    csma2Devices = csma2.Install(csmaNodes2); // install csmanode 2

    // hubungkan ptp ke csma
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes); // ini masih bagong

    // ip config
    InternetStackHelper stack;
    stack.Install(csmaNodes1);
    stack.Install(csmaNodes2);

    //  csma 1
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer csma1interfaces;          // ? change it to router?
    csma1interfaces = address.Assign(csma1Devices);

    // csma 2
    // ipv4AddressHelper address;
    address.SetBase("192.168.17.0", "255.255.255.0");
    Ipv4InterfaceContainer csma2interfaces;
    csma2interfaces = address.Assign(csma2Devices);

    // ptp router
    // ipv4AddressHelper address;
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer routerInterfaces;
    routerInterfaces = address.Assign(p2pDevices);

    // install echoserver to csma2
    UdpEchoServerHelper echoServer (17);
    ApplicationContainer serverApps = echoServer.Install(csmaNodes2);
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // install echoclient to csma1
    UdpEchoClientHelper echoClient(csma2interfaces.GetAddress(0), 17);
    echoClient.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1017));

    // install  udp client di node csma1
    NodeContainer clientNodes (csmaNodes1.Get(0), csmaNodes1.Get(1)); // ???
    

    ApplicationContainer clientApps = echoClient.Install(clientNodes); 
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    csma1.EnablePcap("csma1", csma1Devices);
    csma2.EnablePcap("csma2", csma2Devices);
    pointToPoint.EnablePcapAll("p2p");


    Simulator::Run();
    Simulator::Destroy();
    return 0;

}