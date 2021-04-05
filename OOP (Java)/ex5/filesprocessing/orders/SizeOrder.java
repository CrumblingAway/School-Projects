package filesprocessing.orders;

import java.io.File;
import java.util.ArrayList;

/**
 * Sort files by size
 */
public class SizeOrder extends Order
{
    /**
     * Compares two files based on size
     * @param file1 first file
     * @param file2 second file
     * @return true if file1 < file2, otherwise false
     */
    @Override
    protected boolean itemCompare(File file1, File file2)
    {
        //file1 comes before file2 in a sorted array
        if(file1.length() < file2.length())
        {
            return true;
        }
        //file1 comes after file2 in a sorted array
        else if(file1.length() > file2.length())
        {
            return false;
        }

        //file1 and file2 are equal in size, sort by absolute path
        return file1.getAbsolutePath().compareTo(file2.getAbsolutePath()) <= 0;
    }
}
