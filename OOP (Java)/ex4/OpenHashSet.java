import java.util.Arrays;

public class OpenHashSet extends SimpleHashSet
{
    //Fields
    private WrapperClass[] mainList;

    /**
     * @return The current capacity (number of cells) of the table.
     */
    @Override
    public int capacity()
    {
        return this.ownCap;
    }

    /**
     * Add a specified element to the set if it's not already in it.
     * @param newValue New value to add to the set
     * @return False iff newValue already exists in the set
     */
    @Override
    public boolean add(String newValue)
    {
        if(contains(newValue)) //newValue already in set
        {
            return false;
        }

        int index = clamp(newValue.hashCode());
        this.mainList[index].add(newValue);
        this.size++;

        double ratio = this.size/(double)this.ownCap;
        if(ratio > DEFAULT_HIGHER_CAPACITY)
        {
            rehash(GROW_FACTOR);
        }

        return true;
    }

    /**
     * Look for a specified value in the set.
     * @param searchVal Value to search for
     * @return True iff searchVal is found in the set
     */
    @Override
    public boolean contains(String searchVal)
    {
        int index = clamp(searchVal.hashCode());
        return this.mainList[index].contains(searchVal);
    }

    /**
     * Remove the input element from the set.
     * @param toDelete Value to delete
     * @return True iff toDelete is found and deleted
     */
    @Override
    public boolean delete(String toDelete)
    {
        if(!contains(toDelete)) //toDelete not in set
        {
            return false;
        }

        int index = clamp(toDelete.hashCode());
        this.mainList[index].delete(toDelete);
        this.size--;

        double ratio = (this.size/(double)this.ownCap);
        if(ratio < DEFAULT_LOWER_CAPACITY && this.ownCap != 1)
        {
            rehash(SHRINK_FACTOR);
        }



        return true;
    }

    /**
     * @return The number of elements currently in the set
     */
    @Override
    public int size()
    {
        return this.size;
    }


    /**
     * Creates a new array depending on the resizeFactor (double or half) and
     * rehashes the elements from the old array to the new one.
     * @param resizeFactor decides the size of the new array
     */
    private void rehash(double resizeFactor)
    {
        WrapperClass[] oldArray = this.mainList; //place holder for array

        // Array of new size
        this.ownCap = (int)(this.ownCap*resizeFactor);
        this.size = 0;
        this.mainList = new WrapperClass[this.ownCap];
        for(int i=0; i<this.ownCap; i++)
        {
            this.mainList[i] = new WrapperClass();
        }

        //add items from old array
        for(WrapperClass curList: oldArray)
        {
            for(String element: curList.getLinkedList())
            {
                int index = clamp(element.hashCode());
                this.mainList[index].add(element);
                this.size++;
            }
        }
    }

    //Constructors

    /**
     * Constructs a new, empty table with the specified load factors, and the
     * default initial capacity (16)
     * @param upperLoadFactor The upper load factor of the hash table
     * @param lowerLoadFactor The lower load factor of the hash table
     */
    public OpenHashSet(float upperLoadFactor, float lowerLoadFactor)
    {
        super(upperLoadFactor, lowerLoadFactor);
        this.mainList = new WrapperClass[this.ownCap];
        for(int i=0; i<this.ownCap; i++)
        {
            this.mainList[i] = new WrapperClass();
        }
    }

    /**
     * Data constructor - builds the hash set by adding the elements one by
     * one
     * @param data Values to add to the set
     */
    public OpenHashSet(java.lang.String[] data)
    {
        super();
        this.mainList = new WrapperClass[this.ownCap]; //Create default table
        for(int i=0; i<this.ownCap; i++)
        {
            this.mainList[i] = new WrapperClass();
        }

        for(String datum: data)
        {
            add(datum);
        }
    }

    /**
     * Default constructor
     */
    public OpenHashSet()
    {
        super();
        this.mainList = new WrapperClass[this.ownCap];
        for(int i=0; i<this.ownCap; i++)
        {
            this.mainList[i] = new WrapperClass();
        }
    }
}
