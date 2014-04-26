public class SplashTable{
	/*
	*
	*/
	private int bucketSize;
	private int reInsertions;
	private int tableSize;
	private int numMultipliers;
	
	public static void main(String args[]){
		// Take in arguments
		bucketSize = Integer.parseInt(args[0]);
		reInsertions = Integer.parseInt(args[1]);
		tableSize = Math.pow(Integer.parseInt(args[2]),2);
		numMultipliers = Integer.parseInt(args[3]);
		String inputFile = args[4];
		if(args.size == 7){
			String probeFile = args[6];
			String resultFile = args[7];
		}
		String dumpFile = args[5];
		String probeFile = args[6];
		String resultFile = args[7];
	}
}
