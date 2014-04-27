import java.io.*;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class SplashTable{

	//class members
	private int bucketSize;
	private int numBuckets;
	private int maxReinsertions;
	private int tableSize;
	private int numMultipliers;
	private String inputFile = "";
	private String dumpFile = "";
	private String probeFile = "";
	private String resultFile = "";
	private int[] hashMultipliers;
	private int N; //number of successfully inserted key/payload pairs
	private ArrayList<ArrayList<Integer>> bucketKeys;
	private ArrayList<ArrayList<Integer>> bucketPayloads;

	
	
	/**
	 * Main creates an instance of the SplashTable object and handles all I/O.
	 * Main also calls the build, probe, and dump methods.
	 * @param args
	 */
	public static void main(String args[]){
	
		// Take in arguments assign them to class members
		SplashTable s = new SplashTable();
		s.setBucketSize(Integer.parseInt(args[0]));
		s.setMaxReinsertions(Integer.parseInt(args[1]));
		s.setTableSize((int) Math.pow(2, Integer.parseInt(args[2])));
		s.setNumMultipliers(Integer.parseInt(args[3]));
		s.setInputFile(args[4]);
		s.bucketKeys = new ArrayList<ArrayList<Integer>>();
		s.bucketPayloads = new ArrayList<ArrayList<Integer>>();
		s.setNumBuckets();
		if(args.length == 7){
			s.setProbeFile(args[5]);
			s.setResultFile(args[6]);
		}
		else{
			s.setDumpFile(args[5]);
			s.setProbeFile(args[6]);
			s.setResultFile(args[7]);
		}
		System.out.println(args[5]);

		// Read input file, create list of keys and payloads
		ArrayList<Integer> inputKeys = new ArrayList<Integer>();
		ArrayList<Integer> inputPayloads = new ArrayList<Integer>();
		
		try{
			// Open file that is the first command line parameter
			FileInputStream fstream = new FileInputStream(s.getInputFile());
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			String strLine;
			while ((strLine = br.readLine()) != null) {
				inputKeys.add(Integer.parseInt(strLine.split(" ")[0]));
				inputPayloads.add(Integer.parseInt(strLine.split(" ")[1]));
			}
			in.close();
		}
		catch (Exception e){
			System.err.println("Could not read input file: " + e.getMessage());
		}
		
		// Get numMultipliers hash multipliers
		s.setHashMultipliers(s.getNumMultipliers());
		
		int successfulBuild = s.build(inputKeys, inputPayloads);
		if (successfulBuild == 0){
			System.out.println("Build was not successful.");
			s.printSplashTable();
		}
		else{
			System.out.println("Build was successful.");
			s.printSplashTable();
		}
		
		//will put stuff for probe here
		// Once table has successfully been built
				// set up array of all probes 
		ArrayList<Integer> probeKeys = new ArrayList<Integer>();
				try{
					// Open file that is the first command line parameter
					FileInputStream fstream = new FileInputStream(s.getProbeFile());
					DataInputStream in = new DataInputStream(fstream);
					BufferedReader br = new BufferedReader(new InputStreamReader(in));
					String strLine;
					while ((strLine = br.readLine()) != null) {
						probeKeys.add(Integer.parseInt(strLine));
					}
					in.close();
				}
				catch (Exception e){
					System.err.println("Error: " + e.getMessage());
				}
				// then probe table
				// clear file
				s.clearFile(s.getResultFile());
				// write key and payload to dumpfile
				for(int pKey: probeKeys){
					int payload = 0;
					if((payload = s.probe(pKey)) != 0){
						s.writeResult(s.getResultFile(), pKey, payload);
					}
				}

		//if dumpFile present, output to dumpfile
		if (!s.getDumpFile().equals("")){
			//call dump to get results of dumpfile, then print them out to dumpfile
			ArrayList<String> dumpResults = s.dump();
			
			try{
				File file = new File(s.getDumpFile());
				if (!file.exists()){
					file.createNewFile();
				}
				FileWriter fw = new FileWriter(file);
				BufferedWriter bw = new BufferedWriter(fw);
				for (int i = 0; i < dumpResults.size(); i++){
					bw.write(dumpResults.get(i));
					if (i < dumpResults.size()-1){
						bw.newLine();
					}
				}
				bw.close();
				
			}
			catch (Exception e){
				System.out.println("Could not write dumpfile: " + e.getMessage());
			}
		}
	}
	
	/**
	 * Create hash multipliers - odd numbers in the range [0, 2^32]
	 * @param numMultipliers - the number of hash multipliers to generate
	 */
	private void setHashMultipliers(int numMultipliers){
		 
		hashMultipliers = new int[numMultipliers];
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
				hashMultipliers[i] = randNum;
				uniqueRandom = true;
			}			
			uniqueRandom = false;
		}
		
		//print hash multipliers (for debugging)
		System.out.print("Hash Multipliers: ");
		for (int i = 0; i < hashMultipliers.length; i++){
			System.out.print("h[" + i + "]: " + hashMultipliers[i] + " ");
		}
	}
	
	/**
	 * Build the splash table
	 * @param keys - keys to enter into the splash table
	 * @param payloads - payloads to enter into the splash table
	 * @return 1 for successful build; 0 for failed build
	 */
	private int build(ArrayList<Integer> keys, ArrayList<Integer> payloads){
		
		for (int i = 0; i < keys.size(); i++)
		{
			//TODO: check to make sure value doesn't already exist in bucket
						
			int[] possibleBuckets = generatePossibleBuckets(keys.get(i));
			int successfulInsert = insert(keys.get(i),payloads.get(i), possibleBuckets,0,-1);
			if (successfulInsert == 0)
			{
				//TODO: deal with failed build
				return 0;
			}			
		}
		
		//if cannot be built, return 0
		return 1;
	}
	
	/**
	 * Attempt to insert a key-value pair into the splash table
	 * @param key - the key to insert
	 * @param val - the value/payload to insert
	 * @param possibleBuckets - the buckets to which key may hash
	 * @param attemptedReinsertions - number of reinsertions that have been attempted thus far
	 * @param oldBucketIndex - the previous bucket that this value was in (set to -1 if inserting this pair for the first time)
	 * @return 1 for successful insertion; 0 otherwise
	 */
	private int insert(int key, int val, int[] possibleBuckets, int attemptedReinsertions, int oldBucketIndex){
		
		if(attemptedReinsertions > maxReinsertions){
			System.out.println("was unable to insert key " + key + " and value " + val + ": exceeded maximum reinsertions (" + attemptedReinsertions + ")");
			return 0;	
		}
		
		// attempt to insert key and val into bucket that is not full
		for (int i = 0; i < possibleBuckets.length; i++)
		{
			if (possibleBuckets[i] != oldBucketIndex && bucketKeys.get(possibleBuckets[i]).size() < bucketSize)
			{
				bucketKeys.get(possibleBuckets[i]).add(key);
				bucketPayloads.get(possibleBuckets[i]).add(val);
				assert bucketKeys.get(possibleBuckets[i]).size() == bucketPayloads.get(possibleBuckets[i]).size();
				assert bucketKeys.get(possibleBuckets[i]).size() <= bucketSize;
				System.out.println("Added key " + key + " to bucket " + possibleBuckets[i] + ", number of attempts: " + attemptedReinsertions);
				N++;
				return 1;
			}
		}
		
		//if all possible buckets for the key are full
		int randomBucketIndex = (int)(Math.random()*possibleBuckets.length);
		int randomBucket = possibleBuckets[randomBucketIndex];
		int newKey = bucketKeys.get(randomBucket).remove(0);
		int newVal = bucketPayloads.get(randomBucket).remove(0);
		N--;
		assert (bucketKeys.get(randomBucket).size() == bucketPayloads.get(randomBucket).size());
		assert(bucketKeys.get(randomBucket).size() > 0);
		bucketKeys.get(randomBucket).add(key);
		bucketPayloads.get(randomBucket).add(val);
		N++;
		//System.out.println("Removed key " + newKey + " and value " + newVal + " from bucket " + randomBucket + " to put in key " + key);
		int[] newPossibleBuckets = generatePossibleBuckets(newKey);
		return insert(newKey, newVal, newPossibleBuckets,attemptedReinsertions+1,randomBucket);
		
	}
	
	/**
	 * Generate the possible buckets for the given key based on the hash multipliers
	 * @param key - the key for which to obtain the possible buckets
	 * @return a list of possible buckets for that key
	 */
	private int[] generatePossibleBuckets(int key)
	{
		int[] possibleBuckets = new int[hashMultipliers.length];
		
		for (int j = 0; j < hashMultipliers.length; j++)
		{
			long andOp = (long)Math.pow(2, 32)-1;
			long h = hashMultipliers[j] & andOp;
			long hk = h*((long)key) % (long)Math.pow(2,32);
			String hkStr = Long.toBinaryString(hk);
			int num = (int)(Math.log(numBuckets)/Math.log(2));
			int possibleBucket = (int)(hk >> (hkStr.length()-(num-(32-hkStr.length()))));
			possibleBuckets[j] = possibleBucket;
		}
		
		/*output for debugging
		System.out.print("Possible buckets for key " + key + ":");
		for (int i = 0; i < possibleBuckets.length; i++){
			System.out.print(" h = " + hashMultipliers[i] + ", bucket = " + possibleBuckets[i] + ";");
		}
		System.out.println();*/
		
		return possibleBuckets;
	}
	/*
	 * Probe table , get mask , apply mask to every payload and or them together
	 */
	private int probe(int key){
		int[] hashBuckets = generatePossibleBuckets(key);
		int payload = 0;
		for(int i=0; i<hashMultipliers.length; i++){
			for(int j=0; j< bucketSize; j++){
				int mask = (bucketKeys.get(hashBuckets[i]).get(j)==key) ?  1 : 0;
				// apply mask to each of the payloads and OR all together
				// not sure if this is necessary
				payload |= mask * bucketPayloads.get(hashBuckets[i]).get(j);
			}
		}
		return payload;
	}
	
	/*
	 * Probe table , get mask , apply mask to every payload and or them together
	 */
//	private int probe(int key){
//		int[] hashBuckets = generatePossibleBuckets(key);
//		int payload = 0;
//		for(int i=0; i<hashMultipliers.length; i++){
//			for(int j=0; j< bucketSize; j++){
//				payload = (bucketKeys.get(hashBuckets[i]).get(j)==key) ?  bucketPayloads.get(hashBuckets[i]).get(j) : payload;
//			}
//		}
//		return payload;
//	}
	
	private void dump(String outputFile, int key, int payLoad){
		//Write state of table
		
	}
	
	private void clearFile(String outputFile){
		File resultFile = new File(outputFile); 
		resultFile.delete(); 
		File newResultFile = new File(outputFile); 
		try {
			newResultFile.createNewFile();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	}
	
	private void writeResult(String outputFile, int key, int payLoad){
		//Write state of table
		try{
			  // Create file 
			  FileWriter fstream = new FileWriter(outputFile,true);
			  BufferedWriter out = new BufferedWriter(fstream);
			  out.write(key + " " + payLoad + "\n");
			  //Close the output stream
			  out.close();
			  }catch (Exception e){//Catch exception if any
			  System.err.println("Error: " + e.getMessage());
	   }
	}
	
	/**
	 * Generate a list of all lines in the dump file.
	 * These will be printed to the dumpfile in the main method.
	 * @return a list of a lines to be output to the dump file.
	 */
	private ArrayList<String> dump(){
		ArrayList<String> dumplines = new ArrayList<String>();
		
		//first line: B S h N
		int S = (int)(Math.log(tableSize)/Math.log(2));
		String firstLine = Integer.toString(bucketSize) + " " + Integer.toString(S) + " " +
			Integer.toString(numMultipliers) + " " + Integer.toString(N);
		dumplines.add(firstLine);
		
		//add hash multipliers
		String secondLine = "";
		for (int i = 0; i < hashMultipliers.length; i++){
			secondLine += hashMultipliers[i] + " ";
		}
		dumplines.add(secondLine);
		
		//add key-value pairs in each bucket
		for (int i = 0; i < bucketKeys.size(); i++){
			for (int j = 0; j < bucketKeys.get(i).size(); j++){
				String s = bucketKeys.get(i).get(j) + " " + bucketPayloads.get(i).get(j);
				dumplines.add(s);
			}
		}
		
		return dumplines;
	}
	
	/**
	 * Print the entire splash table
	 */
	private void printSplashTable(){
		System.out.println("N = " + N);
		System.out.println("Bucket\tSlots");
		for (int i = 0; i < bucketKeys.size(); i++){
			ArrayList<Integer> currentBucketKeys = bucketKeys.get(i);
			ArrayList<Integer> currentBucketPayloads = bucketPayloads.get(i);
			System.out.print(i + ":\t");
			for (int j = 0; j < currentBucketKeys.size(); j++){
				System.out.print("(" + currentBucketKeys.get(j) + ", " + currentBucketPayloads.get(j) + ")\t");
			}
			System.out.println("");
		}
	}
	
	// remaining get and set methods follow
		
	private void setBucketSize(int bucketSize){
		this.bucketSize = bucketSize;
	}
	
	private int getBucketSize(){
		return bucketSize;
	}
	
	private void setTableSize(int tableSize){
		this.tableSize = tableSize;
	}
	
	private int getTableSize(){
		return tableSize;
	}	
	
	private void setNumBuckets(){
		assert(tableSize % bucketSize == 0);
		this.numBuckets = (int)(tableSize/bucketSize);
		
		//initialize that many buckets in bucketKeys and bucketPayloads
		for (int i = 0; i < numBuckets; i++){
			this.bucketKeys.add(new ArrayList<Integer>());
			this.bucketPayloads.add(new ArrayList<Integer>());
		}
	}
	
	private void setNumMultipliers(int numMultipliers){
		this.numMultipliers = numMultipliers;
	}
	
	private int getNumMultipliers(){
		return numMultipliers;
	}
	
	private void setMaxReinsertions(int maxReinsertions){
		this.maxReinsertions = maxReinsertions;
	}
	
	private void setN(int N){
		this.N = N;
	}
	private void setInputFile(String inputFile){
		this.inputFile = inputFile;
	}
		
	private String getInputFile(){
		return inputFile;
	}
	
	private void setResultFile(String resultFile){
		this.resultFile = resultFile;
	}
	
	private String getResultFile(){
		return resultFile;
	}
	
	private void setProbeFile(String probeFile){
		this.probeFile = probeFile;
	}
	
	private String getProbeFile(){
		return probeFile;
	}
	
	private void setDumpFile(String dumpFile){
		this.dumpFile = dumpFile;
	}
	
	private String getDumpFile(){
		return dumpFile;
	}
	
}
