#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int
main (int argc, char* argv[])
{	
	NS_LOG_UNCOND("Tugas-2-31");
	Time::SetResolution(Time::NS);
	
	LogComponentEnable ("UdpEchoClienttApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServertApplication", LOG_LEVEL_INFO);
	
	NodeContainer nodes;
	nodes.Create(2);
	
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	
	NetDeviceContainer device;
	device = pointToPoint.Install(nodes);
	
	InternetStackHelper stack;
	stack.Install(nodes);
	
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");
	
	Ipv4InterfaceContainer interfaces = address.Assign(device);
	
	UdpEchoServerHelper echoServer(9);
	
	ApplicationContainer ServerApps = echoServer.Install(nodes.Get(1));
	ServerApps.Start(Seconds(1.0));
	ServerApps.Stop(Seconds(10.0));
	
	UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
	echoClient.SetAttribute("MaxPackets", UintegerValue(1));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1024));
	
	ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));
	
	Simulator::Run();
	Simulator::Destroy();
	return 0;
}
