/*
 * This script simulates a simple network in which one end device sends one
 * packet to the gateway.
 */

#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
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
#include <algorithm>
#include <ctime>

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("ClassCSimpleLorawanNetworkExample");

int main (int argc, char *argv[])
{

// Logging
  //////////

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

  // Create a simple wireless channel
  ///////////////////////////////////

  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  // Helpers
  //////////

  // End Device mobility
  MobilityHelper mobilityEd, mobilityGw;
  Ptr<ListPositionAllocator> positionAllocEd = CreateObject<ListPositionAllocator> ();
  positionAllocEd->Add (Vector (1000.0, 1000.0, 100.0));
  mobilityEd.SetPositionAllocator (positionAllocEd);
  mobilityEd.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // Gateway mobility
  Ptr<ListPositionAllocator> positionAllocGw = CreateObject<ListPositionAllocator> ();
  positionAllocGw->Add (Vector (10.0, 10.0, 10.0));
  mobilityGw.SetPositionAllocator (positionAllocGw);
  mobilityGw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // Create the LoraPhyHelper
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the LorawanMacHelper
  LorawanMacHelper macHelper = LorawanMacHelper ();

  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();

  // Create EDs
  /////////////

  NodeContainer endDevices;
  endDevices.Create (1);
  mobilityEd.Install (endDevices);

  // Create a LoraDeviceAddressGenerator
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (nwkId,nwkAddr);

  // Create the LoraNetDevices of the end devices
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_C);
  macHelper.SetAddressGenerator (addrGen);
  macHelper.SetRegion (LorawanMacHelper::EU);
  helper.Install (phyHelper, macHelper, endDevices);

  // Set message type (Default is unconfirmed)
  Ptr<LorawanMac> edMac1 = endDevices.Get (0)->GetDevice (0)->GetObject<LoraNetDevice> ()->GetMac ();
  Ptr<ClassCEndDeviceLorawanMac> edLorawanMac1 = edMac1->GetObject<ClassCEndDeviceLorawanMac> ();

  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<LoraNetDevice> loraNetDevice = node->GetDevice (0)->GetObject<LoraNetDevice> ();
      Ptr<LoraPhy> phy = loraNetDevice->GetPhy ();
    }

  // Install applications in EDs

  OneShotSenderHelper oneShotHelper = OneShotSenderHelper ();
  oneShotHelper.SetSendTime (Seconds (1));
  oneShotHelper.Install (endDevices.Get (0));
  // oneShotHelper.SetSendTime (Seconds (10));
  // oneShotHelper.Install (endDevices.Get (1));
  // oneShotHelper.SetSendTime (Seconds (8));
  // oneShotHelper.Install(endDevices.Get (1));
  // oneShotHelper.SetSendTime (Seconds (12));
  // oneShotHelper.Install(endDevices.Get (2));

  ////////////////
  // Create GWs //
  ////////////////

  NodeContainer gateways;
  gateways.Create (1);
  mobilityGw.Install (gateways);

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

  FuotaSenderHelper fuotaSenderHelper = FuotaSenderHelper ();
  fuotaSenderHelper.SetSendTime (Seconds (10));
  fuotaSenderHelper.SetDeviceIdAndAddress (nwkId, nwkAddr);
  fuotaSenderHelper.Install (gateways.Get (0));

  // Install the Forwarder application on the gateways
  // ForwarderHelper forwarderHelper;
  // forwarderHelper.Install (gateways);

  // Start simulation
  Simulator::Stop (Seconds (800));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
