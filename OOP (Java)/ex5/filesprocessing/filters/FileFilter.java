package filesprocessing.filters;

import java.io.File;

/**
 * Value is equal to file part
 */
public class FileFilter extends StringFilter implements Filter
{
    /**
     * Compare file part to part
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.getName().equals(part);
    }

    public FileFilter(String fileName)
    {
        part = fileName;
    }
}
