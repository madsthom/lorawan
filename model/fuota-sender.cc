/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include "ns3/fuota-sender.h"
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/class-c-end-device-lorawan-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/lora-net-device.h"
#include "ns3/lora-tag.h"
#include "ns3/uinteger.h"

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("FuotaSender");

NS_OBJECT_ENSURE_REGISTERED (FuotaSender);

TypeId
FuotaSender::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::FuotaSender")
          .SetParent<Application> ()
          .AddConstructor<FuotaSender> ()
          .SetGroupName ("lorawan")
          .AddAttribute ("DataRate", "Data Rate to employ when sending the firmware update",
                         UintegerValue (0), MakeUintegerAccessor (&FuotaSender::m_dataRate),
                         MakeUintegerChecker<uint8_t> (0, 5));
  return tid;
}

FuotaSender::FuotaSender () : m_dataRate (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

FuotaSender::FuotaSender (Time sendTime) : m_sendTime (sendTime), m_dataRate (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

FuotaSender::~FuotaSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
FuotaSender::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
FuotaSender::SetNPacketsToSend (int nPacketsToSend)
{
  NS_LOG_FUNCTION (this << nPacketsToSend);

  m_nPacketsToSend = nPacketsToSend;
}

void FuotaSender::SetDeviceIdAndAddress(uint8_t nwkId, uint32_t nwkAddr)
{
  m_nwkId = nwkId;
  m_nwkAddr = nwkAddr;
}

LoraFrameHeader
FuotaSender::SetFrameHeaderWithFPort(uint8_t fport)
{
  m_message_count++;

  LoraFrameHeader frameHdr;
  frameHdr.SetAsDownlink ();
  frameHdr.SetAck (false);
  frameHdr.SetAdr (true);
  frameHdr.SetFCnt (m_message_count);
  frameHdr.SetFPort(fport);
  frameHdr.SetFPending(0);

  frameHdr.SetAddress (LoraDeviceAddress (m_nwkId, m_nwkAddr));

  return frameHdr;
}

void
FuotaSender::SendPacketOnFPort (uint8_t fport, int packetSize)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> packet = Create<Packet> (packetSize);

  LoraFrameHeader frameHdr = SetFrameHeaderWithFPort (fport);

  packet->AddHeader(frameHdr);

  LorawanMacHeader macHdr;
  macHdr.SetMType(LorawanMacHeader::UNCONFIRMED_DATA_DOWN);
  packet->AddHeader(macHdr);

  // Tag the packet with information about frequency and datarate
  LoraTag tag;
  packet->RemovePacketTag (tag);
  tag.SetDataRate (m_dataRate);
  tag.SetFrequency (869.525);
  packet->AddPacketTag (tag);

  m_mac->Send (packet);
}

void
FuotaSender::SendPacket (void)
{
  NS_LOG_FUNCTION (this);
  SendPacketOnFPort(200, 20);
}

void
FuotaSender::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  m_message_count = 0;

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      NS_LOG_INFO("We have the MAC layer in the FuotaSender");
      // Assumes there's only one device
      Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice (0)->GetObject<LoraNetDevice> ();

      m_mac = loraNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next couple of SendPacket event
  Simulator::Cancel (m_sendEvent);
  m_sendEvent = Simulator::Schedule (m_sendTime, &FuotaSender::SendPacketOnFPort,
                                     this, 205, 20);
  for (int i = 0; i < m_nPacketsToSend; i++)
    {
      m_sendEvent = Simulator::Schedule (m_sendTime + Seconds(14 * i), &FuotaSender::SendPacketOnFPort,
                                      this, 200, 20);
    }
  
  m_sendEvent = Simulator::Schedule (m_sendTime + Seconds(14 * (m_nPacketsToSend + 1)), &FuotaSender::SendPacketOnFPort,
                                     this, 206, 0);
}

void
FuotaSender::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
}
