package filesprocessing.orders;

import filesprocessing.TypeOneException;

/**
 * Factory for creating Order objects
 */
public class OrderFactory
{
    //Magic numbers respective of every order
    private static final String ABS = "abs";
    private static final String TYPE = "type";
    private static final String SIZE = "size";

    /**
     * A factory for creating Order objects.
     * @param values relevant arguments for creating an Order object
     * @return an Order object
     * @throws TypeOneException in case of invalid input
     */
    public Order createOrder(String[] values) throws TypeOneException
    {
        /**
         * Create an Order object based on the given name.
         * In each case the amount of arguments is checked, if found invalid
         * TypeOneException is thrown.
         */
        switch(values[0])
        {
            case ABS:
                if(checkArgs(values))
                {
                    return new AbsOrder();
                }

                throw new TypeOneException();

            case TYPE:
                if(checkArgs(values))
                {
                    return new TypeOrder();
                }

                throw new TypeOneException();

            case SIZE:
                if(checkArgs(values))
                {
                    return new SizeOrder();
                }

                throw new TypeOneException();

            default: //name doesn't match any legal order name
                throw new TypeOneException();
        }
    }

    /**
     * Create an abs order when input is invalid without throwing TypeOneException
     * @return abs order
     */
    public Order createAbsOrder()
    {
        return new AbsOrder();
    }

    /**
     * Check valid amount of arguments for order, which is always either 1 or 2 (including #REVERSE)
     * @param givenArgs given arguments
     * @return true if valid, otherwise false
     */
    private static boolean checkArgs(String[] givenArgs)
    {
        //if the amount of arguments isn't 1 or 2
        if(givenArgs.length < 1 || givenArgs.length > 2)
        {
            return false;
        }

        //if there are two arguments but the second one is not #REVERSE, which is the only valid option
        if(givenArgs.length == 2 && !givenArgs[1].equals("REVERSE"))
        {
            return false;
        }

        return true;
    }
}
