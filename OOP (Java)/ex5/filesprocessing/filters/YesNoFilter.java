package filesprocessing.filters;

/**
 * Abstract class for filters that require a yes/no condition
 */
public abstract class YesNoFilter implements Filter
{
    //The yes and no conditionals
    static final String YES = "YES";
    static final String NO = "NO";

    //Each filter's yes/no value
    protected String cond;
}
