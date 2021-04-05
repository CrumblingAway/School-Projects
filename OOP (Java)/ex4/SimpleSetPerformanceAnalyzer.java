import java.util.HashSet;
import java.util.LinkedList;
import java.util.TreeSet;

public class SimpleSetPerformanceAnalyzer
{
    //Data files converted to arrays of Strings
    private static String[] data1 = Ex4Utils.file2array("/cs/usr/kublakhan/IdeaProjects/ex4/src/data1.txt");
    private static String[] data2 = Ex4Utils.file2array("/cs/usr/kublakhan/IdeaProjects/ex4/src/data2.txt");



    /**
     * Create 5 empty sets
     * @return array of 5 sets
     */
    private static SimpleSet[] makeStructArray()
    {
        return new SimpleSet[]{new OpenHashSet(),
                new ClosedHashSet(),
                new CollectionFacadeSet(new TreeSet<String>()),
                new CollectionFacadeSet(new LinkedList<String>()),
                new CollectionFacadeSet(new HashSet<String>())};
    }

    /**
     * Create 5 sets with data
     * @param data words to put into array
     * @return array of 5 sets with the same data
     */
    private static SimpleSet[] makeStructArray(String[] data)
    {
        SimpleSet[] array = makeStructArray();

        //Open and Closed
        array[0] = new OpenHashSet(data);
        array[1] = new ClosedHashSet(data);

        //Facade structures
        for(int i=2; i < 5; i++)
        {
            for(String word: data)
            {
                array[i].add(word);
            }
        }

        return array;
    }

    /**
     * First and second tests: adding data1/data2 one by one
     * @param set
     */
    private static long testOneTwo(SimpleSet set, String[] data)
    {
        long timeBefore = System.nanoTime(); //start timer

        int counter = 0;

        for(String word: data)
        {
            set.add(word);

            counter++;

            //Msg every ten thousand words added
            if((counter-9999)%10000==0)
                System.out.println(counter + " words complete.");

        }

        return (System.nanoTime() - timeBefore)/1000000;
    }

    /**
     * Measure time and print results for tests one and two (depeinding on data)
     * @param data data for sets
     */
    private static void resultsOneTwo(String[] data)
    {
        SimpleSet[] testArray = makeStructArray();
        long[] results = new long[testArray.length];

        for(int i=0; i < testArray.length; i++)
        {
            results[i] = testOneTwo(testArray[i], data);
        }

        for(int i=0; i < results.length; i++)
        {
            System.out.println((i+1)+") "+results[i]);
        }
    }

    /**
     *Third to sixth tests: contains in data1/data2
     * @param set SimpleSet
     * @param searchValue word to look for
     * @param times warm up repetition
     */
    private static long testThreeToSix(SimpleSet set, String searchValue, int times)
    {
        for(int i=0; i < times; i++)
        {
            set.contains(searchValue);
        }

        long timeBefore = System.nanoTime();

        for(int i=0; i < times; i++)
        {
            set.contains(searchValue);
        }

        return (System.nanoTime() - timeBefore)/times;
    }

    /**
     * Print results for tests three to six
     * @param data data for sets
     * @param searchValue value to search for
     */
    private static void resultsThreeToSix(String[] data, String searchValue)
    {
        final int notLinked = 70000;
        final int linked = 7000;

        SimpleSet[] testArray = makeStructArray(data);
        long[] results = new long[testArray.length];

        for(int i=0; i < 3; i++)
        {
            results[i] = testThreeToSix(testArray[i], searchValue, notLinked);
        }

        results[3] = testThreeToSix(testArray[3], searchValue, linked);
        results[4] = testThreeToSix(testArray[4], searchValue, linked);

        for(int i=0; i < results.length; i++)
        {
            System.out.println((i+1)+") "+results[i]);
        }
    }

    /**
     * Main function
     * @param args
     */
    public static void main(String[] args) {

        System.out.println("First test:");
        resultsOneTwo(data1);
        System.out.println("Second test:");
        resultsOneTwo(data2);
        System.out.println("Third test:");
        resultsThreeToSix(data1, "hi");
        System.out.println("Fourth test:");
        resultsThreeToSix(data1, "-13170890158");
        System.out.println("Fifth test:");
        resultsThreeToSix(data2, "23");
        System.out.println("Sixth test:");
        resultsThreeToSix(data2, "hi");
    }
}
