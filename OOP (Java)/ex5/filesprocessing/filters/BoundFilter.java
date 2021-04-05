package filesprocessing.filters;

/**
 * Abstract class for filters that filter based on upper/lower bound
 */
public abstract class BoundFilter implements Filter
{
    /**
     * Upper/lower bound
     */
    protected double bound;

    /**
     * Ratio to convert bytes to k-bytes
     */
    protected double RATIO = 1024.0;
}
