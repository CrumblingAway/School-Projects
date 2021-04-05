package filesprocessing;

/**
 * Exception class for type 1 exceptions:
 * 1) Bad FILTER/ORDER name
 * 2) Bad parameters.
 */
public class TypeOneException extends Exception
{
    private static final long serialVersionUID = 1L;

    public TypeOneException()
    {
        super();
    }
}
