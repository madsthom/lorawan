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
#include "ns3/network-status.h"
#include "utilities.h"

// An essential include is test.h
#include "ns3/test.h"

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("ClassCEndDeviceLorawanMacTestSuite");

/////////////////////////////
// ClassCEndDeviceLorawanMac testing //
/////////////////////////////

class ClassCEndDeviceLorawanMacTest : public TestCase
{
public:
  ClassCEndDeviceLorawanMacTest ();
  virtual ~ClassCEndDeviceLorawanMacTest ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
ClassCEndDeviceLorawanMacTest::ClassCEndDeviceLorawanMacTest ()
  : TestCase ("Verify correct behavior of the ClassCEndDeviceLorawanMac object")
{
}

// Reminder that the test case should clean up after itself
ClassCEndDeviceLorawanMacTest::~ClassCEndDeviceLorawanMacTest ()
{
}

// This method is the pure virtual method from class TestCase that every
// TestCase must implement
void
ClassCEndDeviceLorawanMacTest::DoRun (void)
{
  NS_LOG_DEBUG ("ClassCEndDeviceLorawanMacTest");

  // Create an ClassCEndDeviceLorawanMac object
  ClassCEndDeviceLorawanMac eds = ClassCEndDeviceLorawanMac ();
}

/////////////////////////////
// ClassCEndDevice testing //
/////////////////////////////

class ClassCEndDeviceTest : public TestCase
{
public:
  ClassCEndDeviceTest ();
  virtual ~ClassCEndDeviceTest ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
ClassCEndDeviceTest::ClassCEndDeviceTest ()
  : TestCase ("Verify correct behavior of the ClassCEndDevice object")
{
}

// Reminder that the test case should clean up after itself
ClassCEndDeviceTest::~ClassCEndDeviceTest ()
{
}


// This method is the pure virtual method from class TestCase that every
// TestCase must implement
void
ClassCEndDeviceTest::DoRun (void)
{
  NS_LOG_DEBUG ("ClassCEndDeviceTest");

}

/**************
 * Test Suite *
 **************/

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run. Typically, only the constructor for
// this class must be defined

class ClassCEndDeviceTestSuite : public TestSuite
{
public:
  ClassCEndDeviceTestSuite ();
};

ClassCEndDeviceTestSuite::ClassCEndDeviceTestSuite ()
  : TestSuite ("class-c-end-device", UNIT)
{
  LogComponentEnable ("ClassCEndDeviceTestSuite", LOG_LEVEL_DEBUG);
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  // AddTestCase (new ClassCEndDeviceLorawanMacTest, TestCase::QUICK);
  AddTestCase (new ClassCEndDeviceTest, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static ClassCEndDeviceTestSuite lorawanTestSuite;
