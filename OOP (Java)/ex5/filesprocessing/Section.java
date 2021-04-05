package filesprocessing;

import filesprocessing.filters.Filter;
import filesprocessing.orders.Order;

/**
 * Holds the information from each section
 */
public class Section
{
    //Filter, Order and errors(if any)
    public Filter filter;
    public Order order;
    int[] errors;

    /**
     * Constructor that sets a filter, order and errors.
     * @param filter given filter
     * @param order given order
     * @param errors given errors (if any)
     */
    public Section(Filter filter, Order order, int[] errors)
    {
        this.filter = filter;
        this.order = order;
        this.errors = errors;
    }
}
