package filesprocessing.filters;

import java.io.File;

/**
 * Value is contained within file name
 */
public class ContainsFilter extends StringFilter
{
    /**
     * Check if part appears within file's name
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.getName().contains(part);
    }

    public ContainsFilter(String segment)
    {
        part = segment;
    }
}
