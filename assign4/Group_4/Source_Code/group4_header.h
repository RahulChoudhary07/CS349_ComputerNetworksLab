/* Network topology

  (H1)n0                                n5(H4)
       \ 100 Mb/s, 10ms                /
        \          10Mb/s, 100ms      /   100 Mb/s, 10ms
(H2)n1---n3(R1)------------------n4(R2)----n6(H5)
        /                             \
       / 100 Mb/s, 10ms                \  100 Mb/s, 10ms 
  (H3)n2                                n7(H6)

*/
// The above diagram illustrates a Dumbbell topology with two routers R1 and R2. Each router is connected to 3 hosts
// R1 is connected to H1,H2,H3 and R2 is //connected to H4,H5,H6. 



/*
Custom application for binding the socket at application initiation time
and to increase the data rate at run time.
This is taken from fifth.cc from ns3 examples.
*/

//Start and end time of applications


class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangeRate(DataRate newrate);


private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

//This function is used to change the data rate of an application
void
MyApp::ChangeRate(DataRate newrate)
{
   m_dataRate = newrate;
   return;
}

//This function is used to increase the data rate of an application
void IncRate (Ptr<MyApp> app, DataRate rate, FlowMonitorHelper *flowMonitorHelp, Ptr<FlowMonitor> flowMonitor, int boolvar)
{
  app->ChangeRate(rate);
  if(boolvar==1)
  {
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMonitorHelp->GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> fmStats = flowMonitor->GetFlowStats ();
	double totFlow = 0, totFlowSquare = 0;
	double totTCP = 0, totUDP=0, totOtherUDP=0;
	double individualTCP[3];
	double individualUDP[3];	
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = fmStats.begin (); i != fmStats.end (); ++i)
	{
		Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
		//Calculating the throughput
		double throughPut = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/(1024*1024);
		totFlow += throughPut;
		totFlowSquare += throughPut * throughPut ;
		// Calculating throughput from TCP connections.
		if(t.sourceAddress == "10.1.1.1" && t.destinationAddress == "10.1.6.2")
		{
		    totTCP += throughPut;
		    individualTCP[0] += throughPut;
		}
		else if(t.sourceAddress == "10.1.1.1" && t.destinationAddress == "10.1.7.2")
		{
		  totTCP += throughPut;
		  individualTCP[1] += throughPut;
		}
		else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.7.2")
		{
	  		totTCP += throughPut;
		    individualTCP[2] += throughPut;
		}
		else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.1.1" )
		{
			totUDP += throughPut;
			totOtherUDP += throughPut;
		    individualUDP[0] += throughPut;
		}
		else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.3.1" )
		{
			totUDP += throughPut;
			totOtherUDP += throughPut;
		    individualUDP[1] += throughPut;
		}
		else if(t.sourceAddress == "10.1.2.1" && t.destinationAddress == "10.1.3.1" )
		{
			totUDP += throughPut;
		    individualUDP[2] += throughPut;
		}
	}
	//Calculating the fairness
	// double FairnessIndex = (totFlow * totFlow)/ (6 * totFlowSquare) ;
	// dataset.Add (varDataRate, FairnessIndex);
	if(totTCP!=0)
	datasetTcp_udprate.Add (varDataRate, totTCP);
	// Throughput from UDP connections= Total throughput- throughput from TCP connections.
	if(totUDP!=0)
	datasetUdp_udprate.Add (varDataRate, totUDP);
	if(totOtherUDP!=0)
	datasetOtherUdp_udprate.Add (varDataRate, totOtherUDP); 
	
	datasetTcp1.Add (varDataRate, individualTCP[0]);
	datasetTcp2.Add (varDataRate, individualTCP[1]);
	datasetTcp3.Add (varDataRate, individualTCP[2]);

	datasetUdp1.Add (varDataRate, individualUDP[0]);
	datasetUdp2.Add (varDataRate, individualUDP[1]);
	datasetUdp3.Add (varDataRate, individualUDP[2]);



	std :: cout << "UDP Data Rate: " << varDataRate << "Mbps\t\t";
	// std :: cout << "FairnessIndex: " << FairnessIndex << "\t\t";
	std :: cout << "TCP Throughput: " << totTCP << "\t\t";
	std :: cout << "UDP Throughput: " << totUDP << "\t\t";
	std :: cout << "Remaining 2 UDP Throughput: " << totOtherUDP << std :: endl;
  }
  varDataRate+=10;


}

//Function for establishing TCP connection between trace source and trace sink.
Ptr<MyApp> establishTCP(NodeContainer &nodes, Ipv4InterfaceContainer &intface, int source, int sink, int bsize)
{
  sinkPort++;
  Address sinkAddress (InetSocketAddress (intface.GetAddress (1), sinkPort));
  PacketSinkHelper pckSink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  //Sink application using packet sink helper which consumes all packets from port1
  ApplicationContainer sinkApps = pckSink.Install (nodes.Get (sink));
  sinkApps.Start (Seconds (start_time_sink));
  sinkApps.Stop (Seconds (run_time));

  //Creating a tcp socket for sender application
  Ptr<Socket> tcpSocket = Socket::CreateSocket (nodes.Get (source), TcpSocketFactory::GetTypeId ());
  tcpSocket->SetAttribute("SndBufSize",  ns3::UintegerValue(bsize));
  tcpSocket->SetAttribute("RcvBufSize",  ns3::UintegerValue(bsize));

  //Creating sender application
  Ptr<MyApp> tcp_Agent = CreateObject<MyApp> ();
  tcp_Agent->Setup (tcpSocket, sinkAddress, packetSize, packetsToSend, DataRate ("20Mbps"));
  nodes.Get (source)->AddApplication (tcp_Agent);
  tcp_Agent->SetStartTime (Seconds (start_time_apps));
  tcp_Agent->SetStopTime (Seconds (run_time));	
  return tcp_Agent;
}

//Function for establishing UDP connection between trace source and trace sink.
Ptr<MyApp> establishUDP(NodeContainer &nodes, Ipv4InterfaceContainer &intface, int source, int sink, int bsize){
    
   //Assign a new port for every new connection    
  sinkPort++;

  //Creating sink
  Address sinkAddress (InetSocketAddress (intface.GetAddress (0), sinkPort));
  PacketSinkHelper pckSink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  //Creating a sink application to consume all packets from the above port
  ApplicationContainer sinkApps = pckSink.Install (nodes.Get (sink));
  sinkApps.Start (Seconds (start_time_sink));
  sinkApps.Stop (Seconds (run_time));

  //Creating socket
  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (nodes.Get (source), UdpSocketFactory::GetTypeId ());
  ns3UdpSocket->SetAttribute("RcvBufSize",  ns3::UintegerValue(bsize));

  //Creating sender application
  Ptr<MyApp> udp_Agent = CreateObject<MyApp> ();
  udp_Agent->Setup (ns3UdpSocket, sinkAddress, packetSize, packetsToSend, DataRate ("20Mbps"));
  nodes.Get (source)->AddApplication (udp_Agent);
  udp_Agent->SetStartTime (Seconds (start_time_apps));
  udp_Agent->SetStopTime (Seconds (run_time));

  return udp_Agent;

}

void getPlot(std :: string fname, std :: string plotTitle, std :: string legendx, std :: string legendy, std :: string extra, Gnuplot2dDataset &dataset)
{
	std :: string gfname        = fname + ".png";
	std :: string pfname        = fname + ".plt";
	Gnuplot plot (gfname);
	plot.SetTitle (plotTitle);
	plot.SetTerminal ("png");
	plot.SetLegend (legendx, legendy);
	plot.AppendExtra (extra);
	plot.AddDataset (dataset);
	std::ofstream plotFile (pfname.c_str());
	plot.GenerateOutput (plotFile);
	plotFile.close ();
}