package filesprocessing.filters;

import java.io.File;

/**
 * Check if file has writing permission
 */
public class WriteFilter extends YesNoFilter
{
    /**
     * Check if file has writing premission
     * @param file file
     * @return true if yes, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        if(this.cond.equals(YES))
        {
            return file.canWrite();
        }else if(this.cond.equals(NO))
        {
            return !file.canWrite();
        }

        return false; //PLACE HOLDER, CHANGE LATER
    }

    public WriteFilter(String value)
    {
        this.cond = value;
    }
}
