#include "ns3/core-module.h"
#include "ns3/network-module.h"

using namespace ns3;

int main(int argc, char* argv[])
{
    /* code */
    NS_LOG_UNCOND("Tugas-2-67");

    NodeContainer node;
    Nodes.Create(2);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
