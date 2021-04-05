import oop.ex3.searchengine.Hotel;
import oop.ex3.searchengine.HotelDataset;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Class for BoopingSite. Can return hotels sorted by rating and proximity
 */
public class BoopingSite
{
    String name;
    Hotel[] hotels;

    /**
     * Returns hotels in given city sorted by rating. If rating is the same,
     * sorts alphabetically.
     * @param city
     * @return sorted array of hotels
     */
    public Hotel[] getHotelsInCityByRating(String city)
    {
        ArrayList<Hotel> interim = makeHotelArrayList(hotels);
        filterByCity(interim, city);
        Collections.sort(interim, new StarComparator());
        Hotel[] result = makeNormalHotelArray(interim);
        return result;
    }

    /**
     * Returns hotels sorted by proximity to given location. If distance is
     * the same, sorts by points of interest.
     * @param latitude
     * @param longitude
     * @return sorted array of hotels
     */
    public Hotel[] getHotelsByProximity(double latitude, double longitude)
    {
        ArrayList<Hotel> interim = makeHotelArrayList(hotels);
        Collections.sort(interim, new ProximityComparator(latitude, longitude));
        Hotel[] result = makeNormalHotelArray(interim);
        return result;
    }

    /**
     * Returns hotels sorted by proximity in a given city. If distance is the
     * same, sorts by points of interest.
     * @param city
     * @param latitude
     * @param longitude
     * @return
     */
    public Hotel[] getHotelsInCityByProximity(String city, double latitude, double longitude)
    {
        ArrayList<Hotel> interim = makeHotelArrayList(hotels);
        filterByCity(interim, city);
        Collections.sort(interim, new ProximityComparator(latitude, longitude));
        Hotel[] result = makeNormalHotelArray(interim);
        return result;
    }

    /**
     * Makes ArrayList of hotels out of Hotel[]
     * @param array
     * @return ArrayList of hotels
     */
    private ArrayList<Hotel> makeHotelArrayList(Hotel[] array)
    {
        ArrayList<Hotel> result = new ArrayList<Hotel>();
        if(array.length==0)
        {
            return result;
        }
        for(int i=0; i<array.length; i++)
        {
            result.add(array[i]);
        }
        return result;
    }

    /**
     * Makes Hotel[] out of ArrayList of hotels
     * @param array
     * @return
     */
    private Hotel[] makeNormalHotelArray(ArrayList<Hotel> array)
    {
        Hotel[] result = new Hotel[array.size()];
        if(array.size()==0)
        {
            return result;
        }
        for(int i=0; i<array.size(); i++)
        {
            result[i] = array.get(i);
        }
        return result;
    }

    /**
     * Filters out hotels not from city
     * @param array
     */
    private void filterByCity(ArrayList<Hotel> array, String city)
    {
        array.removeIf(hotel -> !(hotel.getCity().equals(city)));
    }

    /**
     * Constructor
     * @param name
     */
    public BoopingSite(String name)
    {
        this.name = name;
        this.hotels = HotelDataset.getHotels(this.name);
    }
}