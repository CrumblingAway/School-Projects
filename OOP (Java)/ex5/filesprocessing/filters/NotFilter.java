package filesprocessing.filters;

import java.io.File;

/**
 * Decorator for filters, returns opposite boolean result
 */
public class NotFilter implements Filter
{
    /**
     * Filter that needs decorating, is used for comparing
     */
    private Filter curFilter;

    /**
     * The not filters filter method. Simply returns the opposite
     * of what its curFilter would return.
     * @param file file
     * @return opposite of curFilter.filter()
     */
    @Override
    public boolean filter(File file)
    {
        return !curFilter.filter(file);
    }

    public NotFilter(Filter filter)
    {
        curFilter = filter;
    }
}
