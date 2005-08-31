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
	private static final String[] START_ARGS_SIMPLE = {"start"};
	private static final String[] START_ARGS_MEMORY = {"start", "memory"};
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
			assertFalse("Main invoked with no arguments, not running", SampleService.getServiceInstance().isServiceExecuting());
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
			assertFalse("Main invoked with bad arguments, not running", SampleService.getServiceInstance().isServiceExecuting());
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
			assertFalse("Main invoked with stop argument, not running", SampleService.getServiceInstance().isServiceExecuting());
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main\n" + t);
		}

		assertTrue("Main entry point with stop argument - when not running", calledIt);
	}

	public void testMainStartArgsSimple()
	{
		boolean calledIt = false;
		try
		{
			scheduleStop(3); // end the process that is started below

			SampleService.main(START_ARGS_SIMPLE); // returns when stopped
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main (simple)\n" + t);
		}

		assertTrue("Main entry point with start argument", calledIt);
	}

	public void testMainStartArgsMemory()
	{
		boolean calledIt = false;
		try
		{
			scheduleStop(3); // end the process that is started below

			SampleService.main(START_ARGS_MEMORY); // returns when stopped
			calledIt = true;
		}
		catch (Throwable t)
		{
			fail("Failed during processing of SampleService.main (memory)\n" + t);
		}

		assertTrue("Main entry point with start+memory argument", calledIt);
	}

	public void testStartEntryPoint()
	{
		boolean calledIt = false;
		try
		{
			scheduleStop(5); // end the thread that gets started below

			SampleService.serviceStart(START_ARGS_SIMPLE); // returns when stopped
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
			assertTrue("Stop to be invoked, should be running", SampleService.getServiceInstance().isServiceExecuting());
			SampleService.serviceStop(STOP_ARGS);
			calledIt = true;
			assertFalse("Stop invoked, no longer running", SampleService.getServiceInstance().isServiceExecuting());
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
