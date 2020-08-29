#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstExerciseLogger");

int main(int argc, char* argv[]) {
	CommandLine cmd(__FILE__);
	cmd.Parse(argc, argv);

	PointToPointHelper pointToPointRouter;
	pointToPointRouter.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	pointToPointRouter.SetChannelAttribute("Delay", StringValue("20ms"));

	PointToPointHelper pointToPointLeaf;
	pointToPointLeaf.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	pointToPointLeaf.SetChannelAttribute("Delay", StringValue("40ms"));

	PointToPointDumbbellHelper pointToPointDumbbell (
		2, pointToPointLeaf,
		2, pointToPointLeaf,
		pointToPointRouter
	);

	InternetStackHelper stack;
	pointToPointDumbbell.InstallStack(stack);
	pointToPointDumbbell.AssignIpv4Addresses(
		Ipv4AddressHelper("10.1.1.0", "255.255.255.0"),
		Ipv4AddressHelper("10.2.1.0", "255.255.255.0"),
		Ipv4AddressHelper("10.3.1.0", "255.255.255.0")
	);

	OnOffHelper firstSourceHelper(
		"ns3::TcpSocketFactory",
		Address()
	);

	firstSourceHelper.SetAttribute("DataRate", StringValue("8Mbps"));

	AddressValue firstDestinationAddress(InetSocketAddress(pointToPointDumbbell.GetRightIpv4Address(0), 1000));

	firstSourceHelper.SetAttribute("Remote", firstDestinationAddress);

	PacketSinkHelper firstDestinationHelper(
		"ns3::TcpSocketFactory",
		Address()
	);

	firstDestinationHelper.SetAttribute("Local", firstDestinationAddress);

	ApplicationContainer firstApplications;
	ApplicationContainer firstSinks;

	firstApplications.Add(
		firstSourceHelper.Install(pointToPointDumbbell.GetLeft(0))
	);
	firstSinks.Add(
		firstDestinationHelper.Install(pointToPointDumbbell.GetRight(0))
	);

	firstSinks.Start(Seconds(0.0));
	firstSinks.Stop(Seconds(10.0));

	firstApplications.Start(Seconds(0.0));
	firstApplications.Stop(Seconds(10.0));

	NodeContainer sinkNodes;
	sinkNodes.Add(pointToPointDumbbell.GetRight(0));

	pointToPointRouter.EnablePcap("Ex1", sinkNodes);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
