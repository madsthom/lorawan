/*
 * This script simulates a network in which multiple EDs receive a FUOTA from
 * multiple GWs.
 */

#include "ns3/basic-energy-source-helper.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/file-helper.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
#include "ns3/lora-net-device.h"
#include "ns3/lora-radio-energy-model-helper.h"
#include "ns3/lorawan-mac-header.h"
#include "ns3/object-factory.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rectangle.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/node-container.h"
#include "ns3/position-allocator.h"
#include "ns3/one-shot-sender-helper.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/command-line.h"
#include "ns3/network-server-helper.h"
#include "ns3/fuota-sender-helper.h"
#include "ns3/vector.h"
#include <algorithm>
#include <ctime>
#include <fstream>

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("ClassCScaledUp");

Ptr<FileAggregator> receivedBytesAggregator =
    CreateObject<FileAggregator> ("ReceivedBytes", FileAggregator::FORMATTED);

Ptr<FileAggregator> edStateAggregator =
    CreateObject<FileAggregator> ("EDStates", FileAggregator::FORMATTED);

Ptr<FileAggregator> updateDownloadCompleteAggregator =
    CreateObject<FileAggregator> ("UpdateDownloadComplete", FileAggregator::FORMATTED);

int completedDownloads = 0;
int nGateways = 2;
int nDevices = 1;
int radius = 10e3; // 10 km
std::vector<bool> edFinished (nDevices, false);

void
PrintCoverageRadius (Ptr<PropagationLossModel> lossModel)
{
  std::vector<double> sensitivities = {-124, -127, -130, -133, -135, -137};
  std::vector<double> distances = {0, 0, 0, 0, 0, 0};
  for (double distance = 0; distance < 20000; distance++)
    {
      Ptr<MobilityModel> sender = CreateObjectWithAttributes<ConstantPositionMobilityModel> ();
      sender->SetPosition (Vector3D (0, 0, 0));
      Ptr<MobilityModel> receiver = CreateObjectWithAttributes<ConstantPositionMobilityModel> ();
      receiver->SetPosition (Vector3D (distance, 0, 0));
      double rxPower = lossModel->CalcRxPower (27, sender, receiver);
      for (int i = 0; i < 6; i++)
      {
        if (rxPower > sensitivities.at(i))
        {
          distances.at(i) = distance;
        }
      }
    }

  // Print distances to file
  std::ofstream myfile;
  myfile.open ("ranges.txt");
  for (auto &distance : distances)
    {
      NS_LOG_DEBUG ("Distance: " << distance);
      myfile << distance << std::endl;
    }
  myfile.close ();
}

void
OnUpdateDownloadComplete (Time time)
{
  NS_LOG_FUNCTION (time);

  completedDownloads++;
  edFinished.at (Simulator::GetContext ()) = true;
  updateDownloadCompleteAggregator->Write2d (std::to_string (Simulator::GetContext ()),
                                             Simulator::Now ().GetSeconds (),
                                             Simulator::GetContext ());
  if (std::count (edFinished.begin (), edFinished.end (), true) >= nDevices)
    {
      NS_LOG_DEBUG ("Stopping simulation, since all EDs received their update");
      Simulator::Stop ();
    }
}

void
OnStateChange (EndDeviceLoraPhy::State oldState, EndDeviceLoraPhy::State newState)
{
  NS_LOG_FUNCTION (oldState << newState);

  edStateAggregator->Write3d (std::to_string (Simulator::GetContext ()),
                              Simulator::Now ().GetNanoSeconds (), Simulator::GetContext (),
                              newState);
}

void
OnReceivedPacket (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (packet);

  if (edFinished.at(Simulator::GetContext()))
  {
    return;
  }

  Ptr<Packet> packetCopy = packet->Copy ();
  LorawanMacHeader mHdr;
  mHdr.SetMType (LorawanMacHeader::UNCONFIRMED_DATA_DOWN);
  LoraFrameHeader fHdr;
  packetCopy->RemoveHeader (mHdr);
  packetCopy->RemoveHeader (fHdr);
  packetCopy->RemoveAllPacketTags ();
  packetCopy->RemoveAllByteTags ();

  receivedBytesAggregator->Write3d (std::to_string (Simulator::GetContext ()),
                                    Simulator::Now ().GetNanoSeconds (), Simulator::GetContext (),
                                    packetCopy->GetSize ());
}

void
SwitchToClassA (Ptr<EndDeviceLorawanMac> mac)
{
  mac->GetObject<EndDeviceLorawanMac> ()->m_is_class_c = false;
}

void
SwitchToClassC (Ptr<EndDeviceLorawanMac> mac)
{
  mac->GetObject<EndDeviceLorawanMac> ()->m_is_class_c = true;
}

int
main (int argc, char *argv[])
{

  // Logging
  //////////

  LogComponentEnable ("ClassCScaledUp", LOG_LEVEL_ALL);
  // LogComponentEnable ("GatewayToEnddeviceExample", LOG_LEVEL_ALL);
  // LogComponentEnable ("NetworkServer", LOG_LEVEL_ALL);
  // LogComponentEnable ("NetworkStatus", LOG_LEVEL_ALL);
  LogComponentEnable ("GatewayLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMacHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("MacCommand", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraChannel", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("EndDeviceLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable ("LorawanMacHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("ClassCEndDeviceLorawanMac", LOG_LEVEL_ALL);
  LogComponentEnable ("FuotaSender", LOG_LEVEL_ALL);
  // LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);
  // LogComponentEnable ("Forwarder", LOG_LEVEL_ALL);
  LogComponentEnable ("FuotaSenderHelper", LOG_LEVEL_ALL);
  // LogComponentEnable ("DeviceStatus", LOG_LEVEL_ALL);
  // LogComponentEnable ("GatewayStatus", LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnableAll (LOG_PREFIX_TIME);

  CommandLine cmd;
  cmd.AddValue ("nGateways", "Number of GWs to deploy", nGateways);
  cmd.AddValue ("nDevices", "Number of EDs to deploy", nDevices);
  cmd.AddValue ("radius", "Radius within which to deploy EDs", radius);
  cmd.Parse (argc, argv);

  // Update the edFinished vector
  edFinished  = std::vector<bool> (nDevices, false);

  // Create a simple wireless channel
  ///////////////////////////////////

  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  PrintCoverageRadius(loss);

  //////////////
  // Mobility //
  //////////////

  // End Device mobility
  //////////////////////
  MobilityHelper mobilityEd;
  Ptr<UniformDiscPositionAllocator> positionAllocEd =
      CreateObjectWithAttributes<UniformDiscPositionAllocator> (
          "X", DoubleValue (0), "Y", DoubleValue (0), "Z", DoubleValue (0), "rho",
          DoubleValue (radius));
  mobilityEd.SetPositionAllocator (positionAllocEd);
  // mobilityEd.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityEd.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds",
                               RectangleValue (Rectangle (-radius, radius, -radius, radius)), "Time",
                               TimeValue (Seconds (30)), "Distance", DoubleValue (10), "Speed",
                               PointerValue (CreateObjectWithAttributes<UniformRandomVariable> (
                                   "Min", DoubleValue (2), "Max", DoubleValue (6))));

  // Gateway mobility
  ///////////////////
  MobilityHelper mobilityGw;
  // TODO Use a better position allocator for GWs
  // Using the Hex position allocator could be a good idea here
  Ptr<UniformDiscPositionAllocator> positionAllocGw =
      CreateObjectWithAttributes<UniformDiscPositionAllocator> (
          "X", DoubleValue (0), "Y", DoubleValue (0), "Z", DoubleValue (0), "rho",
          DoubleValue (radius));
  mobilityGw.SetPositionAllocator (positionAllocGw);
  mobilityGw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // LoRaWAN Helpers
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);
  LorawanMacHelper macHelper = LorawanMacHelper ();
  LoraHelper helper = LoraHelper ();

  ////////////////
  // Create EDs //
  ////////////////

  NodeContainer endDevices;
  endDevices.Create (nDevices);
  mobilityEd.Install (endDevices);

  // Create a LoraDeviceAddressGenerator
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen =
      CreateObject<LoraDeviceAddressGenerator> (nwkId, nwkAddr);

  // Create the LoraNetDevices of the end devices
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_C);
  macHelper.SetAddressGenerator (addrGen);
  macHelper.SetRegion (LorawanMacHelper::EU);
  NetDeviceContainer endDeviceNetDevices = helper.Install (phyHelper, macHelper, endDevices);

  // Install applications in EDs

  OneShotSenderHelper oneShotHelper = OneShotSenderHelper ();
  oneShotHelper.SetSendTime (Seconds (1));
  oneShotHelper.Install (endDevices);

  ////////////////
  // Create GWs //
  ////////////////

  NodeContainer gateways;
  gateways.Create (nGateways);
  mobilityGw.Install (gateways);

  // Print the locations of the GWs to file
  std::ofstream myfile;
  myfile.open ("gatewaypositions.txt");
  for (int i = 0; i < nGateways; i++)
    {
      Ptr<MobilityModel> gwMobility = gateways.Get(i)->GetObject<MobilityModel>();
      myfile << gwMobility->GetPosition() << std::endl;
    }
  myfile.close ();

  // Create the LoraNetDevices of the gateways
  phyHelper.SetDeviceType (LoraPhyHelper::GW);
  macHelper.SetDeviceType (LorawanMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  // Set spreading factors up
  macHelper.SetSpreadingFactorsUp (endDevices, gateways, channel);

  ////////////
  // Create NS
  ////////////

  NodeContainer networkServers;
  networkServers.Create (1);

  // // Install the NetworkServer application on the network server
  NetworkServerHelper networkServerHelper;
  networkServerHelper.SetGateways (gateways);
  networkServerHelper.SetEndDevices (endDevices);
  networkServerHelper.Install (networkServers);

  // int nUpdatePackets = 20;

  FuotaSenderHelper fuotaSenderHelper = FuotaSenderHelper ();
  for (int gw = 0; gw < nGateways; gw++)
    {
      fuotaSenderHelper.SetSendTime (Seconds (100 + 2 * gw));
      fuotaSenderHelper.SetDeviceIdAndAddress (nwkId, nwkAddr);
      fuotaSenderHelper.SetNumberOfPacketsToSend (100000); // XXX Never stop sending data
      fuotaSenderHelper.Install (gateways.Get (gw));
    }

  Config::ConnectWithoutContext (
      "/NodeList/*/DeviceList/0/$ns3::LoraNetDevice/Phy/$ns3::EndDeviceLoraPhy/EndDeviceState",
      MakeCallback (&OnStateChange));

  Config::ConnectWithoutContext (
      "/NodeList/*/DeviceList/0/$ns3::LoraNetDevice/Mac/$ns3::EndDeviceLorawanMac/ReceivedPacket",
      MakeCallback (&OnReceivedPacket));

  Config::ConnectWithoutContext (
      "/NodeList/*/ApplicationList/*/$ns3::OneShotSender/UpdateDownloadComplete",
      MakeCallback (&OnUpdateDownloadComplete));

  // Create an aggregator that will have formatted values.
  edStateAggregator->Enable ();
  receivedBytesAggregator->Enable ();

  /************************
   * Install Energy Model *
   ************************/

  BasicEnergySourceHelper basicSourceHelper;
  LoraRadioEnergyModelHelper radioEnergyHelper;

  // Configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (1000)); // Energy in J
  basicSourceHelper.Set ("BasicEnergySupplyVoltageV", DoubleValue (3.3));

  radioEnergyHelper.Set ("StandbyCurrentA", DoubleValue (0.0014));
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.028));
  radioEnergyHelper.Set ("SleepCurrentA", DoubleValue (0.0000015));
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0112));

  radioEnergyHelper.SetTxCurrentModel ("ns3::ConstantLoraTxCurrentModel", "TxCurrent",
                                       DoubleValue (0.028));

  // Install source on EDs' nodes
  EnergySourceContainer sources = basicSourceHelper.Install (endDevices);
  Names::Add ("/Names/EnergySource", sources.Get (0));

  // Install device model
  DeviceEnergyModelContainer deviceModels =
      radioEnergyHelper.Install (endDeviceNetDevices, sources);

  // Print output to file
  FileHelper fileHelper;
  fileHelper.ConfigureFile ("battery-level", FileAggregator::SPACE_SEPARATED);
  fileHelper.WriteProbe ("ns3::DoubleProbe", "/Names/EnergySource/RemainingEnergy", "Output");

  // Install the Forwarder application on the gateways
  // ForwarderHelper forwarderHelper;
  // forwarderHelper.Install (gateways);

  // Start simulation
  Simulator::Stop (Hours (40));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
