import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class SplashTable{

	//class members
	private int bucketSize;
	private int numBuckets;
	private int maxReinsertions;
	private int tableSize;
	private int numMultipliers;
	private String inputFile;
	private String dumpFile;
	private String probeFile;
	private String resultFile;
	private int[] hashMultipliers;
	private ArrayList<ArrayList<Integer>> bucketKeys;
	private ArrayList<ArrayList<Integer>> bucketPayloads;

	
	
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

		// Read input file, create lsit of keys and payloads
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
			System.err.println("Error: " + e.getMessage());
		}
		
		// Get numMultipliers hash multipliers
		s.setHashMultipliers(s.getNumMultipliers());
		
		// Pass variables into build methods
		// key and payloads values 
		// UNCOMMENT THIS TO PASS TO BUILD METHOD
		// Build(inputKeys, inputPayloads);
		System.out.println(inputKeys);
		System.out.println(inputPayloads);
		s.build(inputKeys, inputPayloads);
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
		// write key and payload to dumpfile
		for(int pKey: probeKeys){
			int payload = 0;
			if((payload = s.probe(pKey)) != 0){
				s.writeResult(s.getResultFile(), pKey, payload);
			}
		}
	}
	
	/*
	 * Create hash multipliers -- odd numbers in the range [0, 2^32)
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
	}
	
//	private int build(int[] keys, int[] payloads){
//		
//		for (int i = 0; i < keys.length; i++)
//		{
//			//TODO: check to make sure value doesn't already exist in bucket
//						
//			int[] possibleBuckets = generatePossibleBuckets(keys[i]);
//			int successfulInsert = insert(keys[i],payloads[i], possibleBuckets,0,-1);
//			if (successfulInsert == 0)
//			{
//				//TODO: deal with failed build
//				return 0;
//			}			
//		}
//		
//		//if cannot be built, return 0
//		return 1;
//	}
	
	/*
	 * Needed to rewrite a little because I won't know size to have arrays
	 * need arraylist
	 */
	private int build(ArrayList<Integer> keys, ArrayList<Integer> payloads){
		
		for (int i = 0; i < keys.size(); i++)
		{
			//TODO: check to make sure value doesn't already exist in bucket
						
			int[] possibleBuckets = generatePossibleBuckets(keys.get(i));
			System.out.println("In here" + possibleBuckets.toString());
			int successfulInsert = insert(keys.get(i), payloads.get(i), possibleBuckets,0,-1);
			if (successfulInsert == 0)
			{
				//TODO: deal with failed build
				return 0;
			}			
		}
		
		//if cannot be built, return 0
		return 1;
	}
	
	/*
	 * Attempt to insert the parameters key and val into the splash table. 
	 * Return 1 for successful insertion; return 0 if unsuccessful (i.e. exceeded maximum number of reinsertions)
	 */
	private int insert(int key, int val, int[] possibleBuckets, int attemptedReinsertions, int oldBucketIndex){
		
		if(attemptedReinsertions > maxReinsertions)
			return 0;
		
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
				return 1;
			}
		}
		
		//if all possible buckets for the key are full
		int randomBucketIndex = (int)(Math.random()*possibleBuckets.length);
		int randomBucket = possibleBuckets[randomBucketIndex];
		int newKey = bucketKeys.get(randomBucket).remove(0);
		int newVal = bucketPayloads.get(randomBucket).remove(0);
		assert (bucketKeys.get(randomBucket).size() == bucketPayloads.get(randomBucket).size());
		assert(bucketKeys.get(randomBucket).size() > 0);
		bucketKeys.get(randomBucket).add(key);
		bucketPayloads.get(randomBucket).add(val);
		//System.out.println("Removed key " + newKey + " and value " + newVal + " from bucket " + randomBucket + " to put in key " + key);
		int[] newPossibleBuckets = generatePossibleBuckets(newKey);
		return insert(newKey, newVal, newPossibleBuckets,attemptedReinsertions+1,randomBucket);
		
	}
	
	/*
	 * Generate the possible buckets for a key based on the hash multipliers
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
		
			//System.out.println("Possible bucket for key " + key + " and multiplier " + hashMultipliers[j] + 
			//		" (in long form: " + h + "): " + possibleBucket);
			possibleBuckets[j] = possibleBucket;
		}
		System.out.println("COOL" + possibleBuckets[0]);
		return possibleBuckets;
	}
	/*
	 * Probe table , get mask , apply mask to every payload and or them together
	 */
//	private int probe(int key){
//		int[] hashBuckets = generatePossibleBuckets(key);
//		int payload = 0;
//		for(int i=0; i<hashMultipliers.length; i++){
//			for(int j=0; j< bucketSize; j++){
//				int mask = (bucketKeys.get(hashBuckets[i]).get(j)==key) ?  1 : 0;
//				// apply mask to each of the payloads and OR all together
//				// not sure if this is necessary
//				payload += payload + mask * bucketPayloads.get(hashBuckets[i]).get(j);
//			}
//		}
//		return payload;
//	}
	
	/*
	 * Probe table , get mask , apply mask to every payload and or them together
	 */
	private int probe(int key){
		int[] hashBuckets = generatePossibleBuckets(key);
		int payload = 0;
		for(int i=0; i<hashMultipliers.length; i++){
			for(int j=0; j< bucketSize; j++){
				payload = (bucketKeys.get(hashBuckets[i]).get(j)==key) ?  bucketPayloads.get(hashBuckets[i]).get(j) : payload;
			}
		}
		return payload;
	}
	
	private void dump(String outputFile, int key, int payLoad){
		//Write state of table
		
	}
	
	private void writeResult(String outputFile, int key, int payLoad){
		//Write state of table
		
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
	
	private void setInputFile(String inputFile){
		this.inputFile = inputFile;
	}
		
	private String getInputFile(){
		return inputFile;
	}
	
	private void setResultFile(String resultFile){
		this.resultFile = inputFile;
	}
	
	private String getResultFile(){
		return resultFile;
	}
	
	private void setProbeFile(String ProbeFile){
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
