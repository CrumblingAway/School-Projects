import oop.ex3.searchengine.Hotel;

import java.util.Comparator;

/**
 * Class for comparing hotels by star rating
 */
public class StarComparator implements Comparator<Hotel>
{
    @Override
    public int compare(Hotel hotel1, Hotel hotel2) {
        int compareResult = hotel2.getStarRating() - hotel1.getStarRating();

        if (compareResult != 0) {
            return compareResult;
        }
        return (hotel1.getPropertyName().compareToIgnoreCase(hotel2.getPropertyName()));
    }
}
