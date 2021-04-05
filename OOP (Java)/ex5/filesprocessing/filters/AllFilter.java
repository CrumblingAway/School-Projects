package filesprocessing.filters;

import java.io.File;

/**
 * Default filter, matches every file
 */
public class AllFilter implements Filter
{
    /**
     * No filter
     * @param file file
     * @return always true
     */
    @Override
    public boolean filter(File file)
    {
        return true;
    }
}
