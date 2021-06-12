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
 * Author: Mads Smed <mthoma18@student.aau.dk>
 */

#ifndef FUOTA_SENDER_H
#define FUOTA_SENDER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/lorawan-mac.h"
#include "ns3/attribute.h"
#include "ns3/lora-frame-header.h"

namespace ns3 {
namespace lorawan {

class FuotaSender : public Application
{
public:
  FuotaSender ();
  FuotaSender (Time sendTime);
  ~FuotaSender ();

  static TypeId GetTypeId (void);

  /**
   * Send a packet using the LoraNetDevice's Send method.
   */
  void SendPacket (void);

  void SendPacketOnFPort (uint8_t fport);

  /**
   * Set the time at which this app will send a packet.
   */
  void SetSendTime (Time sendTime);

  void SetDeviceIdAndAddress (uint8_t nwkId, uint32_t nwkAddr);

  LoraFrameHeader SetFrameHeaderWithFPort (uint8_t fport);

  /**
   * Start the application by scheduling the first SendPacket event.
   */
  void StartApplication (void);

  /**
   * Stop the application.
   */
  void StopApplication (void);

private:
  /**
   * The time at which to send the packet.
   */
  Time m_sendTime;

  uint8_t m_nwkId;

  uint32_t m_nwkAddr;

  /**
   * The sending event.
   */
  EventId m_sendEvent;

  /**
   * The MAC layer of this node.
   */
  Ptr<LorawanMac> m_mac;
  uint8_t m_message_count;
};

} //namespace ns3

}
#endif /* ONE_SHOT_APPLICATION */
