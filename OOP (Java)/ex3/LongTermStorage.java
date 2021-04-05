import java.util.HashMap;
import oop.ex3.spaceship.Item;
import java.util.ArrayList;

/**
 * Class for the USS Discovery's long term storage. All items which cannot
 * be stored in the lockers are instead moved here.
 */
public class LongTermStorage implements Storable
{
    final static String NOTENOUGHSPACE = "Error: your request cannot be completed at this time. Problem: no room for n items of type ";
    final static String NISNEGATIVE = "Error: your request cannot be completed at this time. Problem: cannot add a negative number of items of type ";

    /**
     * The long term storage's capacity
     */
    final int capacity;

    ArrayList<Item> items = new ArrayList<Item>();

    /**
     * Adds n items of type item. Prints appropriate message in case of
     * error.
     * @param item
     * @param n
     * @return 0 if successful, -1 if unsuccessful.
     */
    public int addItem(Item item, int n)
    {
        if(n < 0)
        {
            System.out.println(NISNEGATIVE);
            return -1;
        }
        if(n*item.getVolume() > this.getAvailableCapacity())
        {
            System.out.println(NOTENOUGHSPACE + "\""+item.getType()+"\"");
            return -1;
        }
        for(int i=0; i<n; i++)
        {
            this.items.add(item);
        }
        return 0;
    }

    /**
     * Resets the storage's inventory, i.e. after invoking this method the
     * storage becomes empty.
     */
    public void resetInventory()
    {
        if(this.items.size()==0)
        {
            return;
        }
        while(this.items.size() != 0)
        {
            this.items.remove(0);
        }
    }

    /**
     *
     * @param type
     * @return
     */
    public int getItemCount(String type)
    {
        int counter = 0;
        if(this.items.size() != 0)
        {
            for(int i=0; i<this.items.size(); i++)
            {
                if(this.items.get(i).getType().equals(type))
                {
                    counter++;
                }
            }
        }
        return counter;
    }

    /**
     * Returns map of all items in long term storage and their respective
     * quantity.
     * @return map detailing all items and their amount.
     */
    public HashMap<String, Integer> getInventory()
    {
        HashMap<String, Integer> inventory = new HashMap<String, Integer>();
        if(this.items.size() != 0)
        {
            for(int i=0; i<this.items.size(); i++)
            {
                String key = this.items.get(i).getType();
                if(inventory.containsKey(key)) //If item type is already registered
                {
                    inventory.put(key, inventory.get(key)+1);
                }
                else
                {
                    inventory.put(key, 1);
                }
            }
        }
        return inventory;
    }

    /**
     * Returns capacity of long term storage.
     * @return capacity of long term storage.
     */
    public int getCapacity()
    {
        return this.capacity;
    }

    /**
     * Returns available (unoccupied) capacity of long term storage.
     * @return avaiable capacity.
     */
    public int getAvailableCapacity()
    {
        int deduct = 0;
        if(this.items.size() != 0)
        {
            for (int i = 0; i < this.items.size(); i++)
            {
                deduct += this.items.get(i).getVolume();
            }
        }
        return (this.capacity - deduct);
    }

    // Constructor
    public LongTermStorage()
    {
        this.capacity = 1000;
    }
}
