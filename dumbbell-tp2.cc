//   TOPOLOGIA DUMBBELL:
//
//   nodo0                    nodo1
//
//   nodo1 --- r0-----r1------nodo2
//
//   nodo2                    nodo0


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

std::string prefix_file_name = "dumbbell-tp2";

//  "TIME VS CWND" + UMBRAL

static bool firstCwnd = true;
static bool firstSshThr = true;
static bool firstRtt = true;
static bool firstRto = true;

static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static Ptr<OutputStreamWrapper> rttStream;
static Ptr<OutputStreamWrapper> rtoStream;
static Ptr<OutputStreamWrapper> inFlightStream;

static uint32_t cWndValue;
static uint32_t ssThreshValue;


static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }
}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer (Time oldval, Time newval)
{
  if (firstRto)
    {
      *rtoStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto = false;
    }
  *rtoStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
InFlightTracer (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
}

static void
TraceRto (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer));
}

static void
TraceInFlight (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer));
}

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

  //Facilita  la creacion de redes punto a punto.
  PointToPointHelper redDumbbell;
  redDumbbell.SetDeviceAttribute  ("DataRate", StringValue ("100kb/s"));
  redDumbbell.SetChannelAttribute ("Delay", StringValue ("100ms"));
  redDumbbell.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10p"));

  //Facilita la creación de una topología dumbbell con enlaces p2p.
  PointToPointDumbbellHelper dumbbell (cantNodosCliente, redDumbbell, cantNodosServidores, redDumbbell, redDumbbell);

  //Utiliza una combinación de enrutamiento estático y enrutamiento global de forma predeterminada.
  InternetStackHelper stack;
  //Se usa para instalar en cada nodo de la topologia dumbbell.
  dumbbell.InstallStack (stack);
  //Asigna direcciones IPV4 a la topologia.
  dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));
  //Facilita la tarea de administrar el enrutamiento global. Crea una base de datos de enrutamiento e inicializa las tablas de 
  //enrutamiento de los nodos en la simulación. Convierte todos los nodos de la simulación en enrutadores.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ApplicationContainer clienteApps;
  ApplicationContainer servidorApps;

  uint32_t nodo0 = 0;
  uint32_t nodo1 = 1;
  uint32_t nodo2 = 2;

  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  // Facilita el trabajo con OnOffApplications (Envio de paquetes).
  OnOffHelper clienteHelper ("ns3::TcpSocketFactory", Address ());
  clienteHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  //Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
  AddressValue remoteAddress (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
  clienteHelper.SetAttribute ("Remote", remoteAddress);
  //Agrego a mi contenedor de APPs-Cliente, el nuevo cliente junto al puntero del nodo Origen.
  clienteApps.Add (clienteHelper.Install (dumbbell.GetLeft (nodo0)));
  //Crea la aplicacion de tipo servidor/receptor, con nodo Destino y puerto.
  PacketSinkHelper servidor ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo1), puerto));
  //Agrego a mi contenedor de APPs-Servidor, el nuevo servidor junto al puntero y nodo Origen (En este caso = a nodo Destino).
  servidorApps.Add (servidor.Install (dumbbell.GetRight (nodo1)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  OnOffHelper clienteHelper2 ("ns3::TcpSocketFactory", Address ());
  clienteHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  AddressValue remoteAddress2 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
  clienteHelper2.SetAttribute ("Remote", remoteAddress2);
  clienteApps.Add (clienteHelper2.Install (dumbbell.GetLeft (nodo1)));

  PacketSinkHelper servidor2 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo2), puerto));
  servidorApps.Add (servidor2.Install (dumbbell.GetRight (nodo2)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application UDP
  OnOffHelper clienteHelper3 ("ns3::UdpSocketFactory", Address ());
  clienteHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  clienteHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  AddressValue remoteAddress3 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  clienteHelper3.SetAttribute ("Remote", remoteAddress3);
  clienteApps.Add (clienteHelper3.Install (dumbbell.GetLeft (nodo2)));

  PacketSinkHelper servidor3 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  servidorApps.Add (servidor3.Install (dumbbell.GetRight (nodo0)));
  //--------------------------------------------------------------------------------------------------------------------------
  //Comienzo y parada de paquetes enviados (Clientes(emisores) -> Servidores(receptores)).
  clienteApps.Start (Seconds (0.0));
  clienteApps.Stop (Seconds (30.0));
  servidorApps.Start (Seconds (0.0));
  servidorApps.Stop (Seconds (100.0));

  //Generacion de pcap's. - Captura de los paquetes de cada enlace (aristas).
    if (tracing)
    {
      redDumbbell.EnablePcapAll ("dumbbell-tp2", false);

      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd, prefix_file_name + "-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, prefix_file_name + "-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto, prefix_file_name + "-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight, prefix_file_name + "-inflight.data");
   
    }


  NS_LOG_INFO ("Comienzo de Simulación.");
  Simulator::Run ();
  NS_LOG_INFO ("Listo.");
  Simulator::Destroy ();

}
