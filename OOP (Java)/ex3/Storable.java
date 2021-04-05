import java.util.HashMap;
import oop.ex3.spaceship.Item;

/**
 * Interface for all classes responsible for storing items.
 */
public interface Storable
{

    /**
     * Add n items of type item, print appropriate message in case of error.
     * @param item
     * @param n
     * @return 0 if successful, -1 if unsuccessful, otherwise 1.
     */
    int addItem(Item item, int n);

    /**
     * Returns amount of type items.
     * @param type
     * @return amount of type items.
     */
    int getItemCount(String type);

    /**
     * Returns a map detailing all items and their respective quantity.
     * @return map of all items and their quantity.
     */
    HashMap<String, Integer> getInventory();

    /**
     * Returns the capacity.
     * @return capacity.
     */
    int getCapacity();

    /**
     * Returns available (unoccupied) capacity.
     * @return available capacity.
     */
    int getAvailableCapacity();
}
