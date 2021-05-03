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

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("FuotaSender");

NS_OBJECT_ENSURE_REGISTERED (FuotaSender);

TypeId
FuotaSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FuotaSender")
    .SetParent<Application> ()
    .AddConstructor<FuotaSender> ()
    .SetGroupName ("lorawan");
  return tid;
}

FuotaSender::FuotaSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

FuotaSender::FuotaSender (Time sendTime)
  : m_sendTime (sendTime)
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

void FuotaSender::SetDeviceIdAndAddress(uint8_t nwkId, uint32_t nwkAddr)
{
  m_nwkId = nwkId;
  m_nwkAddr = nwkAddr;
}

void
FuotaSender::SendPacket (void)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> packet = Create<Packet> (10);

  LoraFrameHeader frameHdr;
  frameHdr.SetAsDownlink ();
  frameHdr.SetAck (false);
  frameHdr.SetAdr (true);
  frameHdr.SetFCnt (1);
  frameHdr.SetFPort(15);
  frameHdr.SetFPending(0);

  frameHdr.SetAddress (LoraDeviceAddress (m_nwkId, m_nwkAddr));
  packet->AddHeader(frameHdr);

  LorawanMacHeader macHdr;
  macHdr.SetMType(LorawanMacHeader::UNCONFIRMED_DATA_DOWN);
  packet->AddHeader(macHdr);

  // Tag the packet with information about frequency and datarate
  LoraTag tag;
  packet->RemovePacketTag (tag);
  tag.SetDataRate (0);
  tag.SetFrequency (869.525);
  packet->AddPacketTag (tag);

  
  m_mac->Send (packet);

  m_sendEvent = Simulator::Schedule (m_sendTime + Seconds(2), &FuotaSender::SendPacket,
                                     this);
}

void
FuotaSender::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      NS_LOG_INFO("We have the MAC layer in the FuotaSender");
      // Assumes there's only one device
      Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice (0)->GetObject<LoraNetDevice> ();

      m_mac = loraNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);
  m_sendEvent = Simulator::Schedule (m_sendTime, &FuotaSender::SendPacket,
                                     this);
}

void
FuotaSender::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
}
