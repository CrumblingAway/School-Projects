package filesprocessing;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Main class for running the entire filtering and sorting program
 */
public class DirectoryProcessor
{

    /**
     * Create an array of files in a directory that aren't directories
     * @param dir directory
     * @return array of files
     */
    private static ArrayList<File> createFileArray(String dir)
    {
        ArrayList<File> result = new ArrayList<>(); //for storing the files

        File database = new File(dir);

        for(File file: database.listFiles()) //only add files (not directories)
        {
            if(file.isFile())
            {
                result.add(file);
            }
        }

        return result;
    }

    /**
     * Main method
     * @param args directory to filter and sort + command file
     */
    public static void main(String[] args){

        //Check if there aren't 2 arguments
        if(args.length != 2)
        {
            System.err.println("ERROR: invalid amount of arguments for main function");
            return;
        }

        String dir = args[0]; //directory to filter/sort
        String cmdFile = args[1]; //command file

        ArrayList<File> files = createFileArray(dir); //only the files from the directory

        /*
        Create an instance of the parser, use it to create the array of sections.
        For every section first print its errors, then print the result from its
        filter and order.
        TypeTwoException and IOException crash the program.
         */
        try
        {
            FileParser parser = new FileParser();

            ArrayList<Section> sections = parser.parseToArray(cmdFile);

            for(Section section: sections)
            {
                //Print errors
                for(int error: section.errors)
                {
                    if(error !=0)
                    {
                        System.err.println("Warning in line " + error);
                    }
                }

                ArrayList<File> bank = new ArrayList<>(); //for storing filtered and ordered files

                //Filter files
                if(!files.isEmpty())
                {
                    for (File object : files) {

                        if (section.filter.filter(object)) {
                            bank.add(object);
                        }
                    }
                }

                //Sort filtered files
                bank = section.order.sort(bank);

                //Print the names of the sorted files
                for(File item: bank)
                {
                    System.out.println(item.getName());
                }

            }

        }
        catch(IOException e1)
        {
            System.err.println("ERROR: IOException");
        }
        catch(TypeTwoException e3)
        {
            System.err.println(e3.getMessage());
            return;
        }

    }
}
