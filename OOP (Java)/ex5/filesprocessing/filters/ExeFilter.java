package filesprocessing.filters;

import java.io.File;

/**
 * Check if file has execution permission
 */
public class ExeFilter extends YesNoFilter
{
    /**
     * Check if file has writing premission
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        if(this.cond.equals(YES)) //files with execution permission
        {
            return file.canWrite();
        }
        else if(this.cond.equals(NO)) //files without execution permission
        {
            return !file.canWrite();
        }

        return false;
    }

    public ExeFilter(String value)
    {
        this.cond = value;
    }
}
