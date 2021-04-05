package filesprocessing;

import filesprocessing.filters.Filter;
import filesprocessing.filters.FilterFactory;
import filesprocessing.filters.NotFilter;
import filesprocessing.orders.Order;
import filesprocessing.orders.OrderFactory;
import filesprocessing.orders.ReverseOrder;

import java.util.ArrayList;
import java.io.*;

/**
 * Parser that goes over the command file
 */
public class FileParser
{
    private ArrayList<Section> result = new ArrayList<>(); //Will hold all sections

    /**
     * Create an array containing all the appropriate sections of a file
     * @param fileName file to go over
     * @return an array of sections
     */
    public ArrayList<Section> parseToArray(String fileName) throws IOException, TypeTwoException
    {
        //Magic numbers
        final String FILTER = "FILTER";
        final String ORDER = "ORDER";

        //Factories
        FilterFactory filterFactory = new FilterFactory();
        OrderFactory orderFactory = new OrderFactory();

        //reader that goes over the cmd file
        BufferedReader reader = new BufferedReader(new FileReader(fileName));

        ArrayList<String> cmdArray = new ArrayList<>(); //array to store all cmd file's lines
        String row; //every row (line) from cmd file

        //Read all the lines of the file and add them to cmdArray
        while((row = reader.readLine()) != null)
        {
            cmdArray.add(row);
        }

        //Initializing filter, order and errors that will be used to construct the relevant section
        Filter tempFilter = null;
        Order tempOrder = null;
        int error1 = 0;
        int error2 = 0;

        //Counter to keep track of sections
        int sectionCounter = 1;

        /*
         * Build sections from command file.
         * Every line is checked to be the last line of the file to either throw
         * a TypeTwoException or finish constructing the section (provided it's a valid section).
         */
        for(int line = 0; line < cmdArray.size(); line++)
        {
            switch(sectionCounter)
            {
                /*
                Line 1 of section.
                Mandatory: has to be "FILTER"
                If yes and the end of the file hasn't been reached,
                advance sectionCounter one step, otherwise throw TypeTwoException
                 */
                case 1:
                    sectionCounter++;

                    if(line == cmdArray.size()-1)
                    {
                        throw new TypeTwoException("ORDER section missing");
                    }

                    if(!cmdArray.get(line).equals(FILTER))
                    {
                        throw new TypeTwoException("FILTER section missing");
                    }
                    break;

                /*
                Line 2 of section.
                If it's the last line of the command file a TypeTwoException is
                thrown, as a section has to have at least 3 lines.
                Can be anything at all. If it's a valid input for a filter then
                the appropriate filter is created, otherwise the default "all"
                filter is constructed.
                 */
                case 2:
                    sectionCounter++;

                    if(line == cmdArray.size()-1)
                    {
                        throw new TypeTwoException("ORDER section missing");
                    }

                    try
                    {
                        tempFilter = filterFactory.createFilter(cmdArray.get(line).split("#"));

                        if(cmdArray.get(line).endsWith("NOT"))
                        {
                            tempFilter = new NotFilter(tempFilter);
                        }
                    }
                    catch(TypeOneException e)
                    {
                        error1 = line+1;
                        tempFilter = filterFactory.createAllFilter();
                    }

                    break;

                /*
                Line 3 of section.
                Mandatory: has to be "ORDER".
                Again checked for last line, however if that's the case and the line is
                "ORDER" no exception is thrown and the default "abs" order is created.
                 */
                case 3:
                    sectionCounter++;

                    if(!cmdArray.get(line).equals(ORDER))
                    {
                        throw new TypeTwoException("ORDER section missing");
                    }

                    if(line == cmdArray.size()-1)
                    {
                        tempOrder = orderFactory.createAbsOrder();
                        result.add(new Section(tempFilter, tempOrder, new int[]{error1, error2}));
                    }
                    break;

                /*
                Line 4 of section.
                Does not necessarily have to appear in a section.
                If the line is FILTER and we've reached the end of the file
                then throw TypeTwoException because the next section is invalid.
                 */
                case 4:

                    if(!cmdArray.get(line).equals(FILTER))
                    {
                        try
                        {
                            tempOrder = orderFactory.createOrder(cmdArray.get(line).split("#"));

                            if(cmdArray.get(line).endsWith("REVERSE"))
                            {
                                tempOrder = new ReverseOrder(tempOrder);
                            }
                        }
                        catch(TypeOneException e)
                        {
                            error2 = line+1;
                            tempOrder = orderFactory.createAbsOrder();
                        }

                        result.add(new Section(tempFilter, tempOrder, new int[]{error1, error2}));

                        //Reset counters and errors
                        sectionCounter = 1;
                        error1 = 0;
                        error2 = 0;

                        break;
                    }
                    else
                    {
                        if(line == cmdArray.size()-1)
                        {
                            throw new TypeTwoException("ORDER section missing");
                        }

                        tempOrder = orderFactory.createAbsOrder();
                        result.add(new Section(tempFilter, tempOrder, new int[]{error1, error2}));

                        error1 = 0;
                        error2 = 0;

                        sectionCounter = 2; //set counter to 2 becaues we already have the first line "FILTER"
                    }
            }
        }

        return result; //Return the ArrayList of sections
    }
}
