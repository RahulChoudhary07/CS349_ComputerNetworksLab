#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/gnuplot.h"
using namespace ns3;

static uint16_t sinkPort = 8080;

uint32_t start_time_sink = 0;
uint32_t start_time_apps = 0.5;

//Run time for the applications
uint32_t run_time = 10;
//Packet size 1.5 KB
uint32_t packetSize = 1500;

uint32_t packetsToSend = 1000000;

uint32_t varDataRate = 20;

Gnuplot2dDataset dataset;
Gnuplot2dDataset datasetTcp;
Gnuplot2dDataset datasetUdp;

Gnuplot2dDataset datasetTcp_udprate;
Gnuplot2dDataset datasetUdp_udprate;
Gnuplot2dDataset datasetOtherUdp_udprate;

Gnuplot2dDataset datasetTcp1;
Gnuplot2dDataset datasetTcp2;
Gnuplot2dDataset datasetTcp3;
Gnuplot2dDataset datasetUdp1;
Gnuplot2dDataset datasetUdp2;
Gnuplot2dDataset datasetUdp3;
//header files where all the functions are defined
#include "group4_header.h"

NS_LOG_COMPONENT_DEFINE ("Assignment_4");

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
// R1 is connected to H1,H2,H3 and R2 is connected to H4,H5,H6. 

int main (int argc, char *argv[])
{
	//The smallest measurable time interval is 1 ns
	Time::SetResolution (Time::NS);

	//As mentioned in the question, the deafult configuration is set as TCP New Reno
	Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));


	uint32_t bufferSize;	
	// Instantiate the dataset, set its title, and make the points be
	// plotted along with connecting lines.
	dataset.SetTitle ("Fairness");
	dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetTcp.SetTitle ("TCP Throughput");
	datasetTcp.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetUdp.SetTitle ("UDP Throughput");
	datasetUdp.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetTcp_udprate.SetTitle ("TCP Throughput");
	datasetTcp_udprate.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetUdp_udprate.SetTitle ("UDP Throughput");
	datasetUdp_udprate.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetOtherUdp_udprate.SetTitle ("UDP Throughput of Remaining 2 UDPs");
	datasetOtherUdp_udprate.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetTcp1.SetTitle ("TCP Throughput of Connection 1");
	datasetTcp1.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetTcp2.SetTitle ("TCP Throughput of Connection 2");
	datasetTcp2.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetTcp3.SetTitle ("TCP Throughput of Connection 2");
	datasetTcp3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetUdp1.SetTitle ("UDP Throughput of Connection 1");
	datasetUdp1.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetUdp2.SetTitle ("UDP Throughput of Connection 2");
	datasetUdp2.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	datasetUdp3.SetTitle ("UDP Throughput of Connection 3");
	datasetUdp3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	//buffer size is changed at every iteration and throughput is measured for each value of buffer size.
	for(bufferSize=10*packetSize;bufferSize<=800*packetSize;)
	{ 
		//Creating 8 nodes(6 for the end hosts- H1,H2,H3,H4,H5,H6 and 2 for the routers- R1,R2) 
		NodeContainer nodes;
		nodes.Create (8);

		//Create node containers for every link
		NodeContainer n0_3 = NodeContainer (nodes.Get (0), nodes.Get (3)); //h1r1
		NodeContainer n1_3 = NodeContainer (nodes.Get (1), nodes.Get (3)); //h2r1
		NodeContainer n2_3 = NodeContainer (nodes.Get (2), nodes.Get (3)); //h3r1
		NodeContainer n3_4 = NodeContainer (nodes.Get (3), nodes.Get (4)); //r1r2
		NodeContainer n4_5 = NodeContainer (nodes.Get (4), nodes.Get (5)); //r2h4
		NodeContainer n4_6 = NodeContainer (nodes.Get (4), nodes.Get (6)); //r2h5
		NodeContainer n4_7 = NodeContainer (nodes.Get (4), nodes.Get (7)); //r2h6

		//Install the internet stack(protocols) on the nodes
		InternetStackHelper internet;
		internet.Install (nodes);

		//Create point to point channels between the nodes
		PointToPointHelper p2p;
		//Host to router links
		p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
		p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
		NetDeviceContainer d0_3 = p2p.Install (n0_3);
		NetDeviceContainer d1_3 = p2p.Install (n1_3);
		NetDeviceContainer d2_3 = p2p.Install (n2_3);
		NetDeviceContainer d4_5 = p2p.Install (n4_5);
		NetDeviceContainer d4_6 = p2p.Install (n4_6);
		NetDeviceContainer d4_7 = p2p.Install (n4_7);
		//Router to router links
		//Setting the queueing configuration in the router to router link to DropTailQueue as mentioned in the question.
		p2p.SetQueue ("ns3::DropTailQueue", "MaxSize", QueueSizeValue (QueueSize ("85p")));
		p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
		p2p.SetChannelAttribute ("Delay", StringValue ("100ms"));
		NetDeviceContainer d3_4 = p2p.Install (n3_4);

		//Assign IP addresses to every interface
		Ipv4AddressHelper ipv4;
		ipv4.SetBase ("10.1.1.0", "255.255.255.0");
		Ipv4InterfaceContainer i0_3 = ipv4.Assign (d0_3);
		ipv4.SetBase ("10.1.2.0", "255.255.255.0");
		Ipv4InterfaceContainer i1_3 = ipv4.Assign (d1_3);
		ipv4.SetBase ("10.1.3.0", "255.255.255.0");
		Ipv4InterfaceContainer i2_3 = ipv4.Assign (d2_3);
		ipv4.SetBase ("10.1.4.0", "255.255.255.0");
		Ipv4InterfaceContainer i3_4 = ipv4.Assign (d3_4);
		ipv4.SetBase ("10.1.5.0", "255.255.255.0");
		Ipv4InterfaceContainer i4_5 = ipv4.Assign (d4_5);
		ipv4.SetBase ("10.1.6.0", "255.255.255.0");
		Ipv4InterfaceContainer i4_6 = ipv4.Assign (d4_6);
		ipv4.SetBase ("10.1.7.0", "255.255.255.0");
		Ipv4InterfaceContainer i4_7 = ipv4.Assign (d4_7);

		Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

		/*
		TCP Reno Connections
		*/

		//tcp - h1 to h5
		establishTCP(nodes, i4_6, 0, 6, bufferSize);

		//tcp - h1 to h6
		establishTCP(nodes, i4_7, 0, 7, bufferSize);

		//tcp - h4 to h6
		establishTCP(nodes, i4_7, 5, 7, bufferSize);

		/*
		CBR traffic on UDP
		*/

		FlowMonitorHelper flowmn;
		Ptr<FlowMonitor> fmonitor_ptr = flowmn.InstallAll();

		//udp - h4 to h1
		establishUDP(nodes, i0_3, 5, 0, bufferSize);

		//udp - h4 to h3
		establishUDP(nodes, i2_3, 5, 2, bufferSize);

		//udp - h2 to h3
		Ptr<MyApp> udp_to_change = establishUDP(nodes, i2_3, 1, 2, bufferSize);
		//increasing the UDP data rate upto 100Mbps in discrete time steps for UDP between h2 and h3
		int boolvar;
		if(bufferSize==10*packetSize)
			boolvar=1;
		else
			boolvar=0;
		// Simulator::Schedule (Seconds(1.0), &IncRate, udp_to_change, DataRate("20Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(2.0), &IncRate, udp_to_change, DataRate("30Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(3.0), &IncRate, udp_to_change, DataRate("40Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(4.0), &IncRate, udp_to_change, DataRate("50Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(5.0), &IncRate, udp_to_change, DataRate("60Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(6.0), &IncRate, udp_to_change, DataRate("70Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(7.0), &IncRate, udp_to_change, DataRate("80Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(8.0), &IncRate, udp_to_change, DataRate("90Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(9.0), &IncRate, udp_to_change, DataRate("100Mbps"), &flowmn, fmonitor_ptr, boolvar);
		Simulator::Schedule (Seconds(10.0), &IncRate, udp_to_change, DataRate("100Mbps"), &flowmn, fmonitor_ptr, boolvar);


		NS_LOG_INFO ("Run Simulation.");
		Simulator::Stop (Seconds(10.0));
		Simulator::Run ();

		fmonitor_ptr->CheckForLostPackets ();
		//This is used to calculate the throughput at each second. 
		//Total throughput, throuput from TCP connections and throughput from UDP connections are calculated.
		Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmn.GetClassifier ());
		std::map<FlowId, FlowMonitor::FlowStats> fmStats = fmonitor_ptr->GetFlowStats ();
		double totFlow = 0, totFlowSquare = 0;
		double totTCP = 0, totUDP = 0;
		for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = fmStats.begin (); i != fmStats.end (); ++i)
		{
			Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
			//Calculate the throughput of the flow
			double throughPut = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/(1024*1024);
			totFlow += throughPut;
			totFlowSquare += throughPut * throughPut ;
                        //TCP connection
			if(t.sourceAddress == "10.1.1.1" && t.destinationAddress == "10.1.6.2")
			{
				totTCP += throughPut;
			}
			//TCP connection
			else if(t.sourceAddress == "10.1.1.1" && t.destinationAddress == "10.1.7.2")
			{
				totTCP += throughPut;
			}
			//TCP connection
			else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.7.2")
			{
				totTCP += throughPut;
			}
			//UDP connection
			else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.1.1" )
			{
				totUDP += throughPut;
			}
			//UDP connection
			else if(t.sourceAddress == "10.1.5.2" && t.destinationAddress == "10.1.3.1" )
			{
				totUDP += throughPut;
			}
			//UDP connection
			else if(t.sourceAddress == "10.1.2.1" && t.destinationAddress == "10.1.3.1" )
			{
				totUDP += throughPut;
			}
		}
		//Calculate the fainess index from the throughput
		double FairnessIndex = (totFlow * totFlow)/ (6 * totFlowSquare);
		//Adding to the dataset
		dataset.Add (bufferSize/packetSize, FairnessIndex);
		datasetTcp.Add (bufferSize/packetSize, totTCP);
		datasetUdp.Add (bufferSize/packetSize, totUDP);
		std :: cout << "Buffer Size: " << bufferSize/packetSize << "packets\t\t";
		std :: cout << "FairnessIndex: " << FairnessIndex << "\t\t";
		std :: cout << "TCP Throughput: " << totTCP << "\t\t";
		std :: cout << "UDP Throughput: " << totUDP << std :: endl;

		Simulator::Destroy ();

		if(bufferSize < 100*packetSize) 
			bufferSize+=18*packetSize;
		else 
			bufferSize+=80*packetSize;
	}


	getPlot("group4_fairness_buffersize", "Buffer Size vs Fairness plot", "Buffer Size(packets)", "Fairness", "set xrange [0:800]", dataset);
	getPlot("group4_tcp_buffersize", "Buffer Size vs Throughput(TCP)", "Buffer Size(packets)", "Throughput(Mbps)", "set xrange [0:800]", datasetTcp);
	getPlot("group4_udp_buffersize", "Buffer Size vs Throughput(UDP)", "Buffer Size(packets)", "Throughput(Mbps)", "set xrange [0:800]", datasetUdp);
	
	getPlot("group4_tcp_udprate", "UDP Rate vs Throughput(TCP)", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetTcp_udprate);
	getPlot("group4_udp_udprate", "UDP Rate vs Throughput(UDP)", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetUdp_udprate);
	getPlot("group4_otherudp_udprate", "UDP Rate vs Throughput of remaining 2 UDP connections", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetOtherUdp_udprate);

	getPlot("group4_tcp1_udprate", "UDP Rate vs TCP Throughput of Connection 1", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetTcp1);
	getPlot("group4_tcp2_udprate", "UDP Rate vs TCP Throughput of Connection 2", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetTcp2);
	getPlot("group4_tcp3_udprate", "UDP Rate vs TCP Throughput of Connection 3", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetTcp3);
	getPlot("group4_udp1_udprate", "UDP Rate vs UDP Throughput of Connection 1", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetUdp1);
	getPlot("group4_udp2_udprate", "UDP Rate vs UDP Throughput of Connection 2", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetUdp2);
	getPlot("group4_udp3_udprate", "UDP Rate vs UDP Throughput of Connection 3", "UDP Rate(Mbps)", "Throughput(Mbps)", "set xrange [10:100]", datasetUdp3);


	NS_LOG_INFO ("Done.");
}
