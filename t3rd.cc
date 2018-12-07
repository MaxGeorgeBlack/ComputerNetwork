#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

//      AP
//  *    *                n2------------n3----------n5
//  |    |                 | 
// n5    n0 -------------- n1------------n4----------

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  bool verbose = true;
  uint32_t nCsma = 1;
  uint32_t nWifi = 1;
  bool tracing = false;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer p2pNodes2;
  p2pNodes2.Add (p2pNodes.Get (1));
  p2pNodes2.Create (1);

  NodeContainer p2pNodes3;
  p2pNodes3.Add (p2pNodes2.Get (1));
  p2pNodes3.Create (1);


  NodeContainer p2pNodes4;
  p2pNodes4.Add (p2pNodes3.Get (1));
  p2pNodes4.Create (1);
  
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);
  
  NodeContainer p2pNodes5;
  p2pNodes5.Add (p2pNodes4.Get (1));
  p2pNodes5.Add (csmaNodes.Get (1));
  
  NodeContainer p2pNodes6;
  p2pNodes6.Add (p2pNodes3.Get (0));
  p2pNodes6.Add (p2pNodes3.Get (1));
  p2pNodes6.Add (p2pNodes4.Get (1));

  

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);  
  NetDeviceContainer p2pDevices2;
  p2pDevices2 = pointToPoint.Install (p2pNodes2);  
  NetDeviceContainer p2pDevices3;
  p2pDevices3 = pointToPoint.Install (p2pNodes3);  
  NetDeviceContainer p2pDevices4;
  p2pDevices4 = pointToPoint.Install (p2pNodes4); 
   
  NetDeviceContainer p2pDevices5;
  p2pDevices5 = pointToPoint.Install (p2pNodes5);  
  
  NetDeviceContainer csmaDevices;
  csmaDevices = pointToPoint.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);
  stack.Install (p2pNodes6);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2;
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.12.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3;
  p2pInterfaces3 = address.Assign (p2pDevices3);

  address.SetBase ("10.1.13.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4;
  p2pInterfaces4 = address.Assign (p2pDevices4);
  
  address.SetBase ("10.1.14.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5;
  p2pInterfaces5 = address.Assign (p2pDevices5);
  
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (p2pInterfaces4.GetAddress (1), 9));
  onoff.SetConstantRate (DataRate ("2kbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (50));

  ApplicationContainer apps = onoff.Install (wifiStaNodes.Get (nWifi - 1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (20.0));
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), 9)));
  apps = sink.Install (p2pNodes4.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (20.0));

  Simulator::Stop (Seconds (20.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", apDevices.Get (0));
      //csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }



  Ptr<Node> n1 = csmaNodes.Get(1);
  Ptr<Ipv4> ipv41 = n1->GetObject<Ipv4> ();
  uint32_t ipv4ifIndex1 = 2;

  Simulator::Schedule (Seconds (8),&Ipv4::SetDown,ipv41, ipv4ifIndex1);
  Simulator::Schedule (Seconds (16),&Ipv4::SetUp,ipv41, ipv4ifIndex1);

  Ptr<Node> n2 = p2pNodes4.Get(0);
  Ptr<Ipv4> ipv42 = n2->GetObject<Ipv4> ();

  Simulator::Schedule (Seconds (12),&Ipv4::SetDown,ipv42, ipv4ifIndex1);
  Simulator::Schedule (Seconds (14),&Ipv4::SetUp,ipv42, ipv4ifIndex1);



  AnimationInterface anim ("third.xml");
  anim.SetConstantPosition(wifiStaNodes.Get(0),0.0,36.5);
  anim.SetConstantPosition(wifiApNode.Get(0),5.0,36.5);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
