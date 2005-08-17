package org.objectweb.javaservice.test;

/*
 * Test case to check that SampleService entry points are available.
 *
 * Copyright (c) 2005 Multiplan Consultants Ltd.
 */

import junit.framework.TestCase;

/**
 *
 */
public class TestEntryPoints extends TestCase
{
	private static final String[] NO_ARGS = {};
	private static final String[] BAD_ARGS = {"unknown"};
	private static final String[] START_ARGS = {"start"};
	private static final String[] STOP_ARGS = {"stop"};

	public TestEntryPoints()
	{
		super();
	}
	
	public void setUp()
	{
		// nothing needed here yet	
	}

	public void tearDown()
	{
		// nothing needed here yet	
	}

	public void testMainNoArgs()
	{
		boolean calledIt = false;
		try
		{
			SampleService.main(NO_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main\n" + t);
		}

		assertTrue("Main entry point with no arguments", calledIt);
	}

	public void testMainBadArgs()
	{
		boolean calledIt = false;
		try
		{
			SampleService.main(BAD_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main\n" + t);
		}

		assertTrue("Main entry point with invalid argument(s)", calledIt);
	}

	public void testMainStopArgs()
	{
		boolean calledIt = false;
		try
		{
			SampleService.main(STOP_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main\n" + t);
		}

		assertTrue("Main entry point with stop argument - when not running", calledIt);
	}

	public void testMainStartArgs()
	{
		boolean calledIt = false;
		try
		{
			scheduleStop(3); // end the process that is started below

			SampleService.main(START_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main\n" + t);
		}

		assertTrue("Main entry point with stop argument - when not running", calledIt);
	}

	public void testStartEntryPoint()
	{
		boolean calledIt = false;
		try
		{
			scheduleStop(3); // end the process that is started below

			SampleService.serviceStart(START_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.serviceStart\n" + t);
		}

		assertTrue("Service Start entry point accessible", calledIt);
	}

	private void scheduleStop(int secsDelay)
	{
		new ScheduledStopThread(secsDelay).start();
	}
	
	void callStopEntryPoint()
	{
		boolean calledIt = false;
		try
		{
			SampleService.serviceStop(STOP_ARGS);
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.serviceStop\n" + t);
		}

		assertTrue("Service Stop entry point accessible", calledIt);
	}
	

	class ScheduledStopThread extends Thread
	{
		private int msecsDelay;
		
		ScheduledStopThread(int secs)
		{
			msecsDelay = secs * 1000;
		}
		
		public void run()
		{
			try
			{
				Thread.sleep(msecsDelay);
			}
			catch (InterruptedException ignored)
			{
			}

			callStopEntryPoint();
		}
	}
}
