//
// Sample code to reproduce apparent memory leak from within JavaService
//

/*
If the file writing code is commented out then memory leakage does not 
occur. Admittedly the program does not do anything if the file output stuff 
is not there but I think the file handling stuff is causing the problem.

I know that I could do my logging using the '-out' parameter provided with 
JavaService.exe but the application that I have written is specifically for 
monitoring a directory to which files are ftp'd. I then need to open these 
files and processes them and write response files so I have no other way 
around the problem.

Just a reminder that I am running on Windows 2000 with the classic JVM 
shipped with IBM's Websphere Application Developer.

*/



import java.io.*;

public class OutputTest {

    private static boolean gfi = true;
    private static int counter = 0;
    
	public static void main(String[] args) {

        System.out.println(JavaInfo.getJvmDetails());
        
        OutputTest tst = new OutputTest();

		while(gfi)
		{
			tst.writeToFile();

		}

	}
    
    
    public static void systemExit( String[] args )
    {
        gfi = false;
        System.out.println( "System exit requested (after " + counter + " iterations)" );
        System.out.println(JavaInfo.getHeapDetails());
    }

    
	public void writeToFile()
	{
		String file = "d:\\Multiplan\\dev\\JavaServiceTest\\OutputTest.txt";

		PrintStream out = null;
		try
		{
			out = new PrintStream(new FileOutputStream(file,true));
		}
		catch(IOException ioe)
		{
			System.out.println("IOException caught when creating the printstream " + 
ioe);
		}

		out.println("Hello from JavaService Test (" + (++counter) + ")");
        out.println(JavaInfo.getHeapDetails());
		out.close();


		try
		{
			Thread.sleep(1000);
		}
		catch(InterruptedException ie)
		{
		}

	}
}

