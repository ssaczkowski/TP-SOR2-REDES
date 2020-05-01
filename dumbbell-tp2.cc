// TOPOLOGIA DUMBBELL:
 //
 //   nodo0                    nodo1
 //
 //   nodo1 --- r0-----r1------nodo2
 //
 //   nodo2                    nodo0
 //
 //
 
#include <string>
#include <fstream>
#include <stddef.h>                    
#include <iomanip>                     
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-dumbbell.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE ("TP2-Dumbbell-SOR2");
 
int main (int argc, char *argv[])
{
 uint32_t  cantNodosCliente = 3;
 uint32_t  cantNodosServidores = 3;
 uint32_t puerto = 300;
 bool tracing = true;
 
 
 CommandLine cmd;
 cmd.AddValue ("nLeaf",     "Numero de nodos de cliente y servidores", cantNodosCliente);
 cmd.AddValue ("tracing", "Enable/Disable Tracing", tracing);
 cmd.Parse (argc, argv);
 
 
 PointToPointHelper redDumbbell;
 redDumbbell.SetDeviceAttribute  ("DataRate", StringValue ("100kb/s"));
 redDumbbell.SetChannelAttribute ("Delay", StringValue ("100ms"));
 redDumbbell.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10p"));
 
 PointToPointDumbbellHelper dumbbell (cantNodosCliente, redDumbbell, cantNodosServidores, redDumbbell, redDumbbell);
 
 InternetStackHelper stack;
 dumbbell.InstallStack (stack);
 dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                        Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                        Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));
 
 Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  ApplicationContainer clienteApps;
 ApplicationContainer servidorApps;
 
 
   uint32_t nodo0 = 0;
   uint32_t nodo1 = 1;
   uint32_t nodo2 = 2;
 
   //ON-OFF Application TCP
   OnOffHelper clienteHelper ("ns3::TcpSocketFactory", Address ());
   clienteHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
   clienteHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
 
   AddressValue remoteAddress (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
   clienteHelper.SetAttribute ("Remote", remoteAddress);
   clienteApps.Add (clienteHelper.Install (dumbbell.GetLeft (nodo0)));
 
   PacketSinkHelper servidor ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
   servidorApps.Add (servidor.Install (dumbbell.GetRight (nodo1)));
  
   //ON-OFF Application TCP
   OnOffHelper clienteHelper2 ("ns3::TcpSocketFactory", Address ());
   clienteHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
   clienteHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
 
   AddressValue remoteAddress2 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
   clienteHelper2.SetAttribute ("Remote", remoteAddress2);
   clienteApps.Add (clienteHelper2.Install (dumbbell.GetLeft (nodo1)));
 
   PacketSinkHelper servidor2 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
   servidorApps.Add (servidor2.Install (dumbbell.GetRight (nodo2)));
 
   //ON-OFF Application UDP
   OnOffHelper clienteHelper3 ("ns3::UdpSocketFactory", Address ());
   clienteHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
   clienteHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
 
   AddressValue remoteAddress3 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
   clienteHelper3.SetAttribute ("Remote", remoteAddress3);
   clienteApps.Add (clienteHelper3.Install (dumbbell.GetLeft (nodo2)));
 
   PacketSinkHelper servidor3 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
   servidorApps.Add (servidor3.Install (dumbbell.GetRight (nodo0)));
 
 
 clienteApps.Start (Seconds (0.0));
 clienteApps.Stop (Seconds (30.0));
 servidorApps.Start (Seconds (0.0));
 servidorApps.Stop (Seconds (100.0));
 
 if (tracing)
   {
     redDumbbell.EnablePcapAll ("dumbbell-tp2", false);
   }
 
 NS_LOG_INFO ("Comienzo de Simulación.");
 Simulator::Run ();
 NS_LOG_INFO ("Listo.");
 Simulator::Destroy ();
}
