/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This file includes testing for the following components:
 * - ClassCEndDeviceLorawanMac
 *
 * Author: Binaries group
*/

// Include headers of classes to test
#include "ns3/log.h"
#include "ns3/end-device-status.h"
#include "utilities.h"

// An essential include is test.h
#include "ns3/test.h"

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("LorawanClassCTestSuite");

/////////////////////////////
// ClassCEndDeviceLorawanMac testing //
/////////////////////////////

class LorawanEndDeviceClassCTest : public TestCase
{
public:
  LorawanEndDeviceClassCTest ();
  virtual ~LorawanEndDeviceClassCTest ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
LorawanEndDeviceClassCTest::LorawanEndDeviceClassCTest ()
  : TestCase ("Verify correct behavior of the ClassCEndDeviceLorawanMac object")
{
}

// Reminder that the test case should clean up after itself
LorawanEndDeviceClassCTest::~LorawanEndDeviceClassCTest ()
{
}

// This method is the pure virtual method from class TestCase that every
// TestCase must implement
void
LorawanEndDeviceClassCTest::DoRun (void)
{
  NS_LOG_DEBUG ("LorawanEndDeviceClassCTest");

  // Create an ClassCEndDeviceLorawanMac object
  ClassCEndDeviceLorawanMac eds = ClassCEndDeviceLorawanMac ();
}

/**************
 * Test Suite *
 **************/

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run. Typically, only the constructor for
// this class must be defined

class LorawanClassCTestSuite : public TestSuite
{
public:
  LorawanClassCTestSuite ();
};

LorawanClassCTestSuite::LorawanClassCTestSuite ()
  : TestSuite ("lorawan-class-c", UNIT)
{
  LogComponentEnable ("LorawanClassCTestSuite", LOG_LEVEL_DEBUG);
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new LorawanEndDeviceClassCTest, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static LorawanClassCTestSuite lorawanTestSuite;
