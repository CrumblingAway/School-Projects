import oop.ex3.spaceship.Item;
import oop.ex3.spaceship.ItemFactory;
import org.junit.*;

import static org.junit.Assert.*;
import java.util.HashMap;



/**
 * Test class for Locker.
 */
public class LockerTest
{

    private final static int benderCapacity = 1000;
    private final static int flexoCapacity = 5;

    private static Locker bender;
    private static Locker flexo;
    private static Item baseballBat = ItemFactory.createSingleItem("baseball bat"); // Vol 2
    private static Item helmet1 = ItemFactory.createSingleItem("helmet, size 1"); // Vol 3
    private static Item helmet3 = ItemFactory.createSingleItem("helmet, size 3"); // Vol 5
    private static Item sporesEngine = ItemFactory.createSingleItem("spores engine"); // Vol 10
    private static Item football = ItemFactory.createSingleItem("football"); // Vol 4

    /**
     * Create two new lockers before every test
     */
    @Before
    public void makeLocker()
    {
        bender = new Locker(benderCapacity);
        flexo = new Locker(flexoCapacity);
    }

    /**
     * Confirm the initial capacities
     */
    @Test
    public void testInitCapacity()
    {
        assertEquals("Error: available capacity doesn't match", 1000, bender.getCapacity());
        assertEquals("Error: available capacity doesn't match", 5, flexo.getCapacity());
    }

    /**
     * Add a single item and check capacities
     */
    @Test
    public void testAddItem1()
    {
        //Bender
        assertEquals("Error: failed to add item", 0, bender.addItem(baseballBat, 1));
        assertEquals("Error: max capacity doesn't match", benderCapacity, bender.getCapacity());
        assertEquals("Error: available capacity doesn't match", 998, bender.getAvailableCapacity());
        assertEquals("Error: item count doesn't match", 1, bender.getItemCount("baseball bat"));

        //Flexo
        assertEquals("Error: added item exceeding capacity", -1, flexo.addItem(sporesEngine, 1));
    }

    /**
     * Add items exceeding 50% capacity and check if moved to storage
     */
    @Test
    public void testAddItem2()
    {
        assertEquals("Error: failed to add items", 0, bender.addItem(helmet1, 166));
        bender.addItem(helmet1, 1);
        assertEquals("Error: items were not moved to storage", 66, bender.getItemCount("helmet, size 1"));
    }

    /**
     * Try adding a football to a locker with a baseball bat
     */
    @Test
    public void testAddItem3()
    {
        bender.addItem(baseballBat, 1);
        assertEquals("Error: added football to locker with baseball bat", -2, bender.addItem(football, 1));
    }

    /**
     * Test for removing items
     */
    @Test
    public void testRemoveItem() {
        bender.addItem(helmet1, 100);
        bender.addItem(helmet3, 80);
        bender.addItem(baseballBat, 150);
        assertEquals("Error: available capacity doesn't match", 0, bender.getAvailableCapacity());
        assertEquals("Error: shouldn't be able to remove item not in locker", -1, bender.removeItem(football, 1));
        assertEquals("Error: negative n", -1, bender.removeItem(baseballBat, -1));
        assertEquals("Error: shouldn't be able to remove more items than are in locker", -1, bender.removeItem(helmet1, 101));
        assertEquals("Error: failed to remove items", 0, bender.removeItem(baseballBat, 150));
        assertEquals("Error: available capacity doesn't match", 300, bender.getAvailableCapacity());
        bender.removeItem(helmet3, 80);
        bender.removeItem(helmet1, 100);
        assertEquals("Error: available capacity doesn't match", 1000, bender.getAvailableCapacity());
    }

    /**
     * Add items to locker and confirm item count
     */
    @Test
    public void testGetItemCount()
    {
        bender.addItem(football, 1);
        bender.addItem(helmet3, 2);
        bender.addItem(sporesEngine, 1);
        assertEquals("Error: football count doesn't match", 1, bender.getItemCount("football"));
        assertEquals("Error: helmet3 count doesn't match", 2, bender.getItemCount("helmet, size 3"));
        assertEquals("Error: sporesEngine count doesn't match", 1, bender.getItemCount("spores engine"));
    }

    /**
     * Add items and confirm inventory match
     */
    @Test
    public void testGetInventory()
    {
        bender.addItem(baseballBat, 1);
        bender.addItem(helmet3, 2);
        bender.addItem(sporesEngine, 1);
        HashMap<String, Integer> testMap1 = new HashMap<String, Integer>();
        testMap1.put("baseball bat", 1);
        testMap1.put("helmet, size 3", 2);
        testMap1.put("spores engine", 1);
        assertEquals("Error: map doesn't match inventory", testMap1, bender.getInventory());
        HashMap<String, Integer> testMap2 = new HashMap<String, Integer>();
        bender.removeItem(baseballBat, 1);
        bender.removeItem(helmet3, 2);
        bender.removeItem(sporesEngine, 1);
        assertEquals("Error: map doesn't match inventory", testMap2, bender.getInventory());
    }

    /**
     * Test to see if all items are moved to same storage
     */
    @Test
    public void testSameStorage()
    {
        bender.centralizedStorage.resetInventory();
        bender.addItem(baseballBat, 251);
        flexo.addItem(helmet1, 1);
        assertEquals("Error: baseballBat count doesn't match", 151, bender.centralizedStorage.getItemCount("baseball bat"));
        assertEquals("Error: helmet1 count doesn't match", 1, bender.centralizedStorage.getItemCount("helmet, size 1"));
    }

    public static void main(String[] args) {

    }
}
