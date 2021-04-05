package filesprocessing.filters;

import java.io.File;

/**
 * Check if file is hidden
 */
public class HiddenFilter extends YesNoFilter
{
    /**
     * check if file is hidden
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        if(this.cond.equals(YES)) //hidden files
        {
            return file.isHidden();
        }
        else if(this.cond.equals(NO)) //not hidden files
        {
            return !file.isHidden();
        }

        return false;
    }

    public HiddenFilter(String value)
    {
        this.cond = value;
    }
}
