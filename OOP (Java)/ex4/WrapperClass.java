import java.util.LinkedList;

/**
 * Wrapper class for linked lists for OpenHashSet
 */
public class WrapperClass
{
    LinkedList<String> cellList;

    /**
     * Add a specified element to the set if it's not already in it.
     * @param newValue New value to add to the set
     * @return False iff newValue already exists in the set
     */
    public boolean add(String newValue)
    {
        this.cellList.add(newValue);
        return true;
    }

    /**
     * Look for a specified value in the set.
     * @param searchVal Value to search for
     * @return True iff searchVal is found in the set
     */
    public boolean contains(String searchVal)
    {
        return this.cellList.contains(searchVal);
    }

    /**
     * Remove the input element from the set.
     * @param toDelete Value to delete
     * @return True iff toDelete is found and deleted
     */
    public boolean delete(String toDelete)
    {

        this.cellList.remove(toDelete);
        return true;
    }

    /**
     * @return The number of elements currently in the set
     */
    public int size()
    {
        return this.cellList.size();
    }

    /**
     * @return WrapperClass' linked list field
     */
    public LinkedList<String> getLinkedList()
    {
        return this.cellList;
    }

    /**
     * Constructor
     */
    public WrapperClass()
    {
        this.cellList = new LinkedList<String>();
    }
}
