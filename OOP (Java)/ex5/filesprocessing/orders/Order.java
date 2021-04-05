package filesprocessing.orders;

import java.io.File;
import java.util.ArrayList;

/**
 * Interface for classes that sort files
 */
public abstract class Order
{
    /**
     * Sort given files
     * @return sorted array of files
     */
    public ArrayList<File> sort(ArrayList<File> files)
    {
        QuickSort sorter = new QuickSort(files);
        return sorter.sort(0, files.size()-1);
    }

    /**
     * Sort items in given array
     */
    protected class QuickSort
    {
        ArrayList<File> files; //ArrayList of files to sort (is later returned to caller)

        int partition(int low, int high)
        {
            File pivot = files.get(high);
            int index = low-1;

            for(int j=low; j<high; j++)
            {
                if(itemCompare(files.get(j), pivot))
                {
                    index++;

                    File temp = files.get(index);
                    files.set(index, files.get(j));
                    files.set(j, temp);
                }
            }

            File temp = files.get(index+1);
            files.set(index+1, files.get(high));
            files.set(high, temp);

            return index+1;
        }

        ArrayList<File> sort(int low, int high)
        {
            if(low < high)
            {
                int index = partition(low, high);

                sort(low, index -1);
                sort(index+1, high);
            }

            return files;
        }

        protected QuickSort(ArrayList<File> array)
        {
            files = array;
        }
    }

    protected abstract boolean itemCompare(File file1, File file2);
}
