package filesprocessing.filters;

import java.io.File;

/**
 * Interface for all filters
 */
public interface Filter
{
    /**
     * Run the filter on the requested file
     * @param file file
     * @return true if file meets the filter's requirement, otherwise false
     */
    boolean filter(File file);
}
