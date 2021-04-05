package filesprocessing.filters;

import filesprocessing.TypeOneException;

/**
 * Factory for creating filters
 */
public class FilterFactory
{
    //Magic numbers respective of every filter
    private static final String GREATER_THAN = "greater_than";
    private static final String SMALLER_THAN = "smaller_than";
    private static final String BETWEEN = "between";
    private static final String FILE_NAME = "file";
    private static final String CONTAINS = "contains";
    private static final String PREFIX = "prefix";
    private static final String SUFFIX = "suffix";
    private static final String WRITABLE = "writable";
    private static final String EXECUTABLE = "executable";
    private static final String HIDDEN = "hidden";
    private static final String ALL = "all";
    private static final String NOT = "NOT";

    /**
     * Create filter with parameters
     * @param values name, range/yes/no
     * @return filter
     */
    public Filter createFilter(String[] values) throws TypeOneException
    {
        //Magic numbers for valid amount of arguments per filter
        final int NUM_ARGS = 2;
        final int BET_ARGS = 3;
        final int STRING_ARGS = 2;
        final int PERM_ARGS = 2;
        final int ALL_ARGS = 1;

        /**
         * Switch case for creating different filters.
         * In every case we check whether or not there is even the need to
         * proceed, based on the amount of arguments.
         * A TypeOneException is thrown when appropriate.
         */
        switch(values[0])
        {
            case GREATER_THAN:
                if(checkArgs(NUM_ARGS, values))
                {
                    double greaterVal = Double.parseDouble(values[1]);

                    //bound is not negatve
                    if(greaterVal < 0)
                    {
                        throw new TypeOneException();
                    }

                    return new GreaterFilter(greaterVal);
                }

                throw new TypeOneException();

            case SMALLER_THAN:
                if(checkArgs(NUM_ARGS, values))
                {
                    double smallerVal = Double.parseDouble(values[1]);

                    //bound is positive
                    if(smallerVal < 0)
                    {
                        throw new TypeOneException();
                    }

                    return new SmallerFilter(smallerVal);
                }

                throw new TypeOneException();

            case BETWEEN:
                if(checkArgs(BET_ARGS, values))
                {
                    double minVal = Double.parseDouble(values[1]);
                    double maxVal = Double.parseDouble(values[2]);

                    //if lower bound is greater than upper or if min is negative
                    if(minVal > maxVal || minVal < 0)
                    {
                        throw new TypeOneException();
                    }

                    return new BetweenFilter(minVal, maxVal);
                }

                throw new TypeOneException();

            case FILE_NAME:
                if(checkArgs(STRING_ARGS, values))
                {
                    return new FileFilter(values[1]);
                }

                throw new TypeOneException();

            case CONTAINS:
                if(checkArgs(STRING_ARGS, values))
                {
                    return new ContainsFilter(values[1]);
                }

                throw new TypeOneException();

            case PREFIX:
                if(checkArgs(STRING_ARGS, values))
                {
                    return new PrefixFilter(values[1]);
                }

                throw new TypeOneException();

            case SUFFIX:
                if(checkArgs(STRING_ARGS, values))
                {
                    return new SuffixFilter(values[1]);
                }

                throw new TypeOneException();

            case WRITABLE:
                if(checkArgs(PERM_ARGS, values))
                {
                    return new WriteFilter(values[1]);
                }

                throw new TypeOneException();

            case EXECUTABLE:
                if(checkArgs(PERM_ARGS, values))
                {
                    return new ExeFilter(values[1]);
                }

                throw new TypeOneException();

            case HIDDEN:
                if(checkArgs(PERM_ARGS, values))
                {
                    return new HiddenFilter(values[1]);
                }

                throw new TypeOneException();

            case ALL:
                if(checkArgs(ALL_ARGS, values))
                {
                    return new AllFilter();
                }

            default: //If the given filter name doesn't match any correct name, TypeOneException
                throw new TypeOneException();
        }
    }

    /**
     * Create all filter when input is invalid without needing to throw a TypeOneException
     * @return all filter
     */
    public Filter createAllFilter()
    {
        return new AllFilter();
    }

    /**
     * Check if amount of arguments is valid
     * @param reqArgs required arguments for filter
     * @param givenArgs arguments presented
     * @return true if valid, otherwise false
     */
    private static boolean checkArgs(int reqArgs, String[] givenArgs)
    {
        //not enough/too many arguments
        if(reqArgs > givenArgs.length || givenArgs.length > reqArgs+1)
        {
            return false;
        }

        //Given an extra argument that is not "NOT", which is invalid
        if(givenArgs.length == reqArgs+1 && !givenArgs[givenArgs.length-1].equals(NOT))
        {
            return false;
        }

        return true;
    }
}
