import oop.ex3.searchengine.Hotel;

import java.util.Comparator;

/**
 * Class for comparing hotels by proximity
 */
public class ProximityComparator implements Comparator<Hotel> {

    private double latitude, longitude;

    @Override
    public int compare(Hotel hotel1, Hotel hotel2) {
        double distance1 = getDistance(hotel1);
        double distance2 = getDistance(hotel2);
        double result = distance1 - distance2;

        if(result > 0)
        {
            return 1;
        }
        else if(result < 0)
        {
            return -1;
        }
        return (hotel2.getNumPOI() - hotel1.getNumPOI());
    }

    public double getDistance(Hotel hotel)
    {
        return (Math.sqrt(Math.pow((hotel.getLatitude()-this.latitude), 2) +
                Math.pow((hotel.getLongitude()-this.longitude),2)));
    }

    public ProximityComparator(double latitude, double longitude)
    {
        this.latitude = latitude;
        this.longitude = longitude;
    }
}
