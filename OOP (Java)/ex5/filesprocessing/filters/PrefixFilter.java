package filesprocessing.filters;

import java.io.File;

/**
 * Check if file starts with the part
 */
public class PrefixFilter extends StringFilter
{
    /**
     * Check if file name starts with the part
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.getName().startsWith(part);
    }

    public PrefixFilter(String value)
    {
        part = value;
    }
}
