import java.util.ArrayList;
import java.util.HashMap;
import oop.ex3.spaceship.Item;



/**
 * Class of the lockers on the USS Discovery. Contains methods for managing
 * items, capacity, and checking inventory.
 */
public class Locker implements Storable
{
    final static String NOTENOUGHITEMS = "Error: your request cannot be completed at this time. Problem: locker does not contain n items of type ";
    final static String NISNEGATIVE = "Error: your request cannot be completed at this time. Problem: cannot remove a negative number of items of type ";
    final static String NISNEGATIVE2 = "Error: your request cannot be completed at this time. Problem: cannot add a negative number of items of type ";
    final static String FOOTBALLERROR = "Error: your request cannot be completed at this time. Problem: the locker cannot contain items of type \"football\" as it contains a contradicting item";
    final static String BASEBALLERROR = "Error: your request cannot be completed at this time. Problem: the locker cannot contain items of type \"baseball\" bat as it contains a contradicting item";
    final static String NOTENOUGHSPACE = "Error: your request cannot be completed at this time. Problem: no room for n items of type ";
    final static String NOSPACESTORAGE = "Error: your request cannot be completed at this time. Problem: no room for n* items of type ";
    final static String ITEMSTOSTORAGE = "Warning: action successful, but has caused items to be moved to storage";

    static LongTermStorage centralizedStorage = new LongTermStorage();

    /**
     * The locker's capacity.
     */
    final int capacity;

    /**
     * ArrayList for storing the items.
     */
    ArrayList<Item> items = new ArrayList<Item>();

    /**
     * Adds n items of type item to locker. Prints appropriate messages in
     * case of errors,
     * @param item
     * @param n
     * @return 0 if successful, -1 if unsuccessful, 1 if successful but
     * moved items to storage, -2 if unsuccessful due to conflicting items
     */
    public int addItem(Item item, int n)
    {
        HashMap<String, Integer> currentInventory = this.getInventory();

        if(n < 0)
        {
            System.out.println(NISNEGATIVE2);
            return -1;
        }

        //Check for football/baseball contradiction
        if(item.getType().equals("football"))
        {
            if(currentInventory.containsKey("baseball bat"))
            {
                System.out.println(FOOTBALLERROR);
                return -2;
            }
        }
        else if(item.getType().equals("baseball bat"))
        {
            if(currentInventory.containsKey("football"))
            {
                System.out.println(BASEBALLERROR);
                return -2;
            }
        }

        //If there isn't enough space for n items
        if(n*item.getVolume() > this.getAvailableCapacity())
        {
            System.out.println(NOTENOUGHSPACE+"\""+item.getType()+"\"");
            return -1;
        }

        //If new amount exceeds 50% of capacity
        if((n+this.getItemCount(item.getType()))*item.getVolume() > this.capacity/2)
        {
            //Amount of items that makes up no more than 20% capacity
            int newAmount = this.capacity/5/item.getVolume();
            int oldAmount = this.getItemCount(item.getType());
            if(centralizedStorage.getAvailableCapacity()/item.getVolume() < newAmount)
            {
                System.out.println(NOSPACESTORAGE+"\""+item.getType()+"\"");
                return -1;
            }
            else
            {
                if(oldAmount < newAmount)
                {
                    for (int i = 0; i < newAmount - oldAmount; i++) {
                        this.items.add(item);
                    }
                }
                else
                {
                    this.removeItem(item, oldAmount-newAmount);
                }
                centralizedStorage.addItem(item, n+oldAmount-newAmount);
                System.out.println(ITEMSTOSTORAGE);
                return 1;
            }
        }

        // Enough space, no contradiction, doesn't exceed 50% capacity
        for(int j=0; j<n; j++)
        {
            this.items.add(item);
        }
        return 0;
    }

    /**
     * Removes n items of type item. Prints appropriate messages in case
     * of errors.
     * @param item
     * @param n
     * @return 0 if successful, -1 if unsuccessful.
     */
    public int removeItem(Item item, int n)
    {
        if(n < 0) //If n is negative
        {
            System.out.println(NISNEGATIVE+"\""+item.getType()+"\"");
            return -1;
        }

        //If there are less than n items
        if(n > getItemCount(item.getType()))
        {
            System.out.println(NOTENOUGHITEMS+"\""+item.getType()+"\"");
            return -1;
        }

        if(this.items.size() != 0)
        {
            for(int i=0; i<n; i++)
            {
                this.items.remove(item);
            }
        }
        return 0;
    }

    /**
     * Returns number of type items in locker.
     * @param type
     * @return number of type items in locker
     */
    public int getItemCount(String type)
    {
        int count = 0;
        if(this.items.size() != 0)
        {
            for(int i=0; i<this.items.size(); i++)
            {
                if(this.items.get(i).getType().equals(type))
                {
                    count++;
                }
            }
        }
        return count;
    }

    /**
     * Returns map of all items in locker and their respective quantity.
     * @return map detailing items and their amount.
     */
    public HashMap<String, Integer> getInventory()
    {
        HashMap<String, Integer> inventory = new HashMap<String, Integer>();
        if(this.items.size() != 0) //If no items skip check
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
     * Returns capacity of locker.
     * @return capacity of locker.
     */
    public int getCapacity()
    {
        return this.capacity;
    }

    /**
     * Returns locker's available capacity.
     * @return locker's available capacity.
     */
    public int getAvailableCapacity()
    {
        int deduct = 0;
        if(this.items.size() != 0) // If list is empty skip adding volumes
        {
            for (int i = 0; i < this.items.size(); i++) {
                deduct += this.items.get(i).getVolume();
            }
        }
        return (this.capacity-deduct);
    }

    // Constructor
    public Locker(int capacity)
    {
        this.capacity = capacity;
    }
}
