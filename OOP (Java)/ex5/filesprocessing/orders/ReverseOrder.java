package filesprocessing.orders;

import java.io.File;
import java.util.ArrayList;

/**
 * Decorator for Order objects that reverses the sort order
 */
public class ReverseOrder extends Order
{

    /**
     * Order that needs decorating, is used for sorting
     */
    private Order curOrder;

    /**
     * Use curOrder to filter between two files and return the opposite result
     * @param file1
     * @param file2
     * @return
     */
    @Override
    protected boolean itemCompare(File file1, File file2) {
        return !curOrder.itemCompare(file1, file2);
    }

    public ReverseOrder(Order order)
    {
        curOrder = order;
    }
}
