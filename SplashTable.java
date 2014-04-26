import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class SplashTable{
	private static int bucketSize;
	private static int reInsertions;
	private static int tableSize;
	private static int numMultipliers;
	private static String dumpFile;
	private String probeFile;
	private String resultFile;
	private static ArrayList<Integer> hashMultipliers;

	
	
	public static void main(String args[]){
		// Take in arguments assign them to global variables
		bucketSize = Integer.parseInt(args[0]);
		reInsertions = Integer.parseInt(args[1]);
		tableSize = (int) Math.pow(2, Integer.parseInt(args[2]));
		numMultipliers = Integer.parseInt(args[3]);
		String inputFile = args[4];
//		if(args.length == 7){
//			String probeFile = args[5];
//			String resultFile = args[6];
//		}
//		else{
//			String dumpFile = args[5];
//			String probeFile = args[6];
//			String resultFile = args[7];
//		}
		// Read input file
		// Create key arraylist
		ArrayList<Integer> inputKeys = new ArrayList<Integer>();
		// Create payload arraylist
		ArrayList<Integer> inputPayloads = new ArrayList<Integer>();
		// input file is of format: key file per line 
		try{
			  // Open the file that is the first 
			  // command line parameter
			  FileInputStream fstream = new FileInputStream(inputFile);
			  // Get the object of DataInputStream
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  String strLine;
			  //Read File Line By Line
			  while ((strLine = br.readLine()) != null)   {
			  // Tockenize each line to get seperate key and payload
				  inputKeys.add(Integer.parseInt(strLine.split(" ")[0]));
			  // Add key to keys arraylist
				  inputPayloads.add(Integer.parseInt(strLine.split(" ")[1]));
			  }
			  //Close the input stream
			  in.close();
			    }
		catch (Exception e){//Catch exception if any
			  System.err.println("Error: " + e.getMessage());
			  }
		// Get numMultipliers hash multipliers
		hashMultipliers = hashMultipliers(numMultipliers);
		// Pass variables into build methods
		// key and payloads values 
		// UNCOMMENT THIS TO PASS TO BUILD METHOD
		// Build(inputKeys, inputPayloads);
	}
	
	public static ArrayList<Integer> hashMultipliers(int numMultipliers){
		// 
		ArrayList<Integer> hashMultipliers = new ArrayList<Integer>();
		// change to 0 if necessary
		int min = 1;
		int max = (int) Math.pow(2, 32);
		for(int i=0; i<numMultipliers; i++){
			boolean uniqueRandom = false;
			while(!uniqueRandom){
				int randNum = min + 2*(int)(Math.random()*((max-min)/2+1));
				for(int multiplier: hashMultipliers){
					if(randNum == multiplier){
						break;
					}
				}
				hashMultipliers.add(randNum);
				uniqueRandom = true;
			}			
			uniqueRandom = false;
		}
		return hashMultipliers;
	}
}