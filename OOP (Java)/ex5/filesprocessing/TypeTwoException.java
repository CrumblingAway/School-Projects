package filesprocessing;

/**
 * Exception class for type 2 exceptions:
 * 1) More than two cmd arguments.
 * 2) I/O problems.
 * 3) Bad subsection name (FILTER/ORDER)
 * 4) Bad format of Commands File.
 */
public class TypeTwoException extends Exception
{
    private static final long serialVersionUID = 1L;

    /**
     * Receives message to print an informative message about the program crashing
     * @param msg supplied informative message
     */
    public TypeTwoException(String msg)
    {
        super("ERROR: " + msg);
    }
}
