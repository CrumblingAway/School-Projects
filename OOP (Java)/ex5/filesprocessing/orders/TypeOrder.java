package filesprocessing.orders;

import java.io.File;
import java.util.ArrayList;

/**
 * Sort files alphabetically by type
 */
public class TypeOrder extends Order
{
    /**
     * Use sorting algorithm to sort array
     */
    @Override
    public ArrayList<File> sort(ArrayList<File> files)
    {
        QuickSort sorter = new QuickSort(files);
        ArrayList<File> result = sorter.sort(0, files.size()-1);

        return result;
    }

    /**
     * Compare files based on type
     */
    @Override
    protected boolean itemCompare(File file1, File file2) {

        String name1 = file1.getAbsolutePath(); //file1 path
        String name2 = file2.getAbsolutePath(); //file2 path

        int index1 = name1.lastIndexOf("."); //last period in file1 name
        int index2 = name2.lastIndexOf("."); //last period in file2 name

        String type1 = name1.substring(index1+1); //file1 type
        String type2 = name2.substring(index2+1); //file2 type

        //file1 comes before file2 in a sorted array
        if(type1.compareTo(type2) < 0)
        {
            return true;
        }
        //file1 comes after file2 in a sorted array
        else if(type1.compareTo(type2) > 0)
        {
            return false;
        }

        //file1 and file2 are equal in type, filter based on absolute path
        return file1.getAbsolutePath().compareTo(file2.getAbsolutePath()) <= 0;
    }
}
