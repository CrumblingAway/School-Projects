import oop.ex3.spaceship.Item;
import oop.ex3.spaceship.ItemFactory;
import org.junit.*;

import static org.junit.Assert.*;
import java.util.HashMap;

/**
 * Test class for LongTermStorage.
 */
public class LongTermTest
{
    private static LongTermStorage bank;
    private static Item baseballBat = ItemFactory.createSingleItem("baseball bat"); // Vol 2
    private static Item helmet1 = ItemFactory.createSingleItem("helmet, size 1"); // Vol 3
    private static Item helmet3 = ItemFactory.createSingleItem("helmet, size 3"); // Vol 5
    private static Item sporesEngine = ItemFactory.createSingleItem("spores engine"); // Vol 10
    private static Item football = ItemFactory.createSingleItem("football"); // Vol 4

    @Before
    public void createStorage()
    {
        bank = new LongTermStorage();
    }

    @Test
    public void testAddItem1()
    {
        assertEquals("Error: failed to add items", 0, bank.addItem(football, 150));
        assertEquals("Error: available capacity doesn't match", 400, bank.getAvailableCapacity());
        assertEquals("Error: failed to add items", 0, bank.addItem(sporesEngine, 20));
        assertEquals("Error: available capacity doesn't match", 200, bank.getAvailableCapacity());
        assertEquals("Error: failed to add items", 0, bank.addItem(helmet3, 40));
        assertEquals("Error: available capacity doesn't match", 0, bank.getAvailableCapacity());
        assertEquals("Error: added item despite lack of capacity", -1, bank.addItem(helmet1, 1));
    }

    @Test
    public void testGetItemCount()
    {
        assertEquals("Error: failed to add items", 0, bank.addItem(football, 150));
        assertEquals("Error: item count for football doesn't match", 150, bank.getItemCount("football"));
        assertEquals("Error: failed to add items", 0, bank.addItem(sporesEngine, 20));
        assertEquals("Error: item count for spores engine doesn't match", 20, bank.getItemCount("spores engine"));
        assertEquals("Error: failed to add items", 0, bank.addItem(helmet3, 40));
        assertEquals("Error: item count for helmet3 doesn't match", 40, bank.getItemCount("helmet, size 3"));
    }

    @Test
    public void testGetInventory()
    {
        bank.addItem(football, 150);
        bank.addItem(sporesEngine, 20);
        bank.addItem(helmet3, 40);
        HashMap<String, Integer> testMap1 = new HashMap<String, Integer>();
        testMap1.put("football", 150);
        testMap1.put("spores engine", 20);
        testMap1.put("helmet, size 3", 40);
        assertEquals("Error: map doesn't match inventory", testMap1, bank.getInventory());
        HashMap<String, Integer> testMap2 = new HashMap<String, Integer>();
        bank.resetInventory();
        assertEquals("Error: map doesn't match inventory", testMap2, bank.getInventory());
        assertEquals("Error: storage isn't actually empty", 1000, bank.getAvailableCapacity());
    }

    public static void main(String[] args) {

    }
}
