

/*
 * Filename: JavaInfo.java
 * Created : 04-Aug-03
 * Project : JavaServiceTest
 *
 * Copyright (c) 2003 Multiplan Consultants Ltd. All Rights Reserved. See www.multiplan.co.uk
 * 
 * $Header: /tmp/javaservice.23354/JavaServiceTest/src/JavaInfo.java,v 1.1.1.1 2004-10-30 10:18:14 rutthenut Exp $
 */

/**
 * tba
 * 
 * @version $Revision: 1.1.1.1 $ $Name: not supported by cvs2svn $
 * @author john
 */
public class JavaInfo
{
    public static String getJvmDetails()
    {
        return "JVM " + 
                System.getProperty( "java.vm.vendor", "{vm.vendor}" )
                + " " +
                System.getProperty( "java.vm.name", "{vm.name}" )
                + " " +
                System.getProperty( "java.vm.version", "{vm version}" );
    }
    
    public static String getHeapDetails()
    {
        Runtime rt = Runtime.getRuntime();
        long totalBytes = rt.totalMemory();
        long freeBytes = rt.freeMemory();
        rt = null;

        return "HEAP Size " + kBytes( totalBytes ) + " total, free " + kBytes( freeBytes );    
    }


    private static String kBytes( long bytes )
    {
        if (bytes > 1024)
        {
            bytes -= (bytes % 1024); // rounding to get whole KB figures
        }

        long kb = (long) bytes / 1024;

        return Long.toString( kb ) + "kb";
    }


}
