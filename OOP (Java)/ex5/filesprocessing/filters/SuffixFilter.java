package filesprocessing.filters;

import java.io.File;

/**
 * Check if file ends with the part
 */
public class SuffixFilter extends StringFilter
{
    /**
     * Check if file name ends with the part
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.getName().endsWith(part);
    }

    public SuffixFilter(String value)
    {
        part = value;
    }
}
