package filesprocessing.orders;

import java.io.File;
import java.util.ArrayList;

/**
 * Sort files alphabetically by absolute path
 */
public class AbsOrder extends Order
{
    /**
     * Compare files based on absolute path (comparator)
     */
    @Override
    protected boolean itemCompare(File file1, File file2)
    {
        String name1 = file1.getAbsolutePath(); //file1 path
        String name2 = file2.getAbsolutePath(); //file2 path

        return name1.compareTo(name2) <= 0;
    }
}
