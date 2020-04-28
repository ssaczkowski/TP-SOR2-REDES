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

static bool firstCwnd20 = true;
static bool firstSshThr20 = true;
static bool firstRtt20 = true;
static bool firstRto20 = true;

static Ptr<OutputStreamWrapper> cWndStream20;
static Ptr<OutputStreamWrapper> ssThreshStream20;
static Ptr<OutputStreamWrapper> rttStream20;
static Ptr<OutputStreamWrapper> rtoStream20;
static Ptr<OutputStreamWrapper> inFlightStream20;

static uint32_t cWndValue20;
static uint32_t ssThreshValue20;


static bool firstCwnd30 = true;
static bool firstSshThr30 = true;
static bool firstRtt30 = true;
static bool firstRto30 = true;

static Ptr<OutputStreamWrapper> cWndStream30;
static Ptr<OutputStreamWrapper> ssThreshStream30;
static Ptr<OutputStreamWrapper> rttStream30;
static Ptr<OutputStreamWrapper> rtoStream30;
static Ptr<OutputStreamWrapper> inFlightStream30;

static uint32_t cWndValue30;
static uint32_t ssThreshValue30;

static void
CwndTracer20 (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd20)
    {
      *cWndStream20->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd20 = false;
    }
  *cWndStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue20 = newval;

  if (!firstSshThr20)
    {
      *ssThreshStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue20 << std::endl;
    }
}

static void
CwndTracer30 (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd30)
    {
      *cWndStream30->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd30 = false;
    }
  *cWndStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue30 = newval;

  if (!firstSshThr30)
    {
      *ssThreshStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue30 << std::endl;
    }
}

static void
SsThreshTracer20 (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr20)
    {
      *ssThreshStream20->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr20 = false;
    }
  *ssThreshStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue20 = newval;

  if (!firstCwnd20)
    {
      *cWndStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue20 << std::endl;
    }
}

static void
SsThreshTracer30 (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr30)
    {
      *ssThreshStream30->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr30 = false;
    }
  *ssThreshStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue30 = newval;

  if (!firstCwnd30)
    {
      *cWndStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue30 << std::endl;
    }
}

static void
RttTracer20 (Time oldval, Time newval)
{
  if (firstRtt20)
    {
      *rttStream20->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt20 = false;
    }
  *rttStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RttTracer30 (Time oldval, Time newval)
{
  if (firstRtt30)
    {
      *rttStream30->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt30 = false;
    }
  *rttStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer20 (Time oldval, Time newval)
{
  if (firstRto20)
    {
      *rtoStream20->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto20 = false;
    }
  *rtoStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer30 (Time oldval, Time newval)
{
  if (firstRto30)
    {
      *rtoStream30->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto30 = false;
    }
  *rtoStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
InFlightTracer20 (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream20->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
InFlightTracer30 (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream30->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
TraceCwnd20 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream20 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer20));
}

static void
TraceSsThresh20 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream20 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer20));
}

static void
TraceRtt20 (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream20 = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer20));
}

static void
TraceRto20 (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream20 = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer20));
}

static void
TraceInFlight20 (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream20 = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer20));
}


static void
TraceCwnd30 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream30 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer30));
}

static void
TraceSsThresh30 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream30 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer30));
}

static void
TraceRtt30 (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream30 = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer30));
}

static void
TraceRto30 (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream30 = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer30));
}

static void
TraceInFlight30 (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream30 = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight", MakeCallback (&InFlightTracer30));
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

  //Elijo el algoritmo de congestión de TCP NewReno
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));//TcpWestwood//TcpHighSpeed//TcpHybla

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
  //OnOffHelper clienteHelper3 ("ns3::UdpSocketFactory", Address ());
  //clienteHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  //clienteHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  //AddressValue remoteAddress3 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  //clienteHelper3.SetAttribute ("Remote", remoteAddress3);
  //clienteApps.Add (clienteHelper3.Install (dumbbell.GetLeft (nodo2)));

  //PacketSinkHelper servidor3 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  //servidorApps.Add (servidor3.Install (dumbbell.GetRight (nodo0)));
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

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd20, prefix_file_name + "-20-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh20, prefix_file_name + "-20-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt20, prefix_file_name + "-20-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto20, prefix_file_name + "-20-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight20, prefix_file_name + "-20-inflight.data");


       Simulator::Schedule (Seconds (0.00001), &TraceCwnd30, prefix_file_name + "-30-cwnd.data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh30, prefix_file_name + "-30-ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt30, prefix_file_name + "-30-rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto30, prefix_file_name + "-30-rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight30, prefix_file_name + "-30-inflight.data");
   
    }


  NS_LOG_INFO ("Comienzo de Simulación.");
  Simulator::Run ();
  NS_LOG_INFO ("Listo.");
  Simulator::Destroy ();

}
