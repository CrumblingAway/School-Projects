package filesprocessing.filters;

import java.io.File;

/**
 * File size is smaller than the given number of bytes
 */
public class SmallerFilter extends BoundFilter
{
    /**
     * Compare file size to bound
     * @param file file
     * @return true if greater, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.length()/RATIO < bound;
    }

    public SmallerFilter(double value)
    {
        bound = value;
    }
}
