import oop.ex3.searchengine.Hotel;
import org.junit.*;

import static org.junit.Assert.*;


/**
 * Class for testing BoopingSite
 */
public class BoopingSiteTest {

    private static BoopingSite site1;
    private static BoopingSite site2;
    private static double latitude, longitude;
    private static String city;

    @BeforeClass
    public static void createSite()
    {
        site1 = new BoopingSite("hotels_tst1.txt");
        site2 = new BoopingSite("hotels_tst2.txt");
        latitude = 0.0;
        longitude = 0.0;
        city = "manali";
    }

    @Test
    public void testbyRating1()
    {
        Hotel[] testList = site2.getHotelsInCityByRating(city);
        assertEquals("Error: did not return an empty array", 0, testList.length);
    }

    @Test
    public void testByRating2()
    {
        Hotel[] testList = site1.getHotelsInCityByRating(city);
        for(int i=0; i<testList.length-1; i++)
        {
            assertEquals("Error: hotel isn't from Manali", city, testList[i].getCity());
            assertTrue("Error: hotels are in wrong rating order: " + i + " and " + (i+1), testList[i].getStarRating()>=testList[i+1].getStarRating());
            if(testList[i].getStarRating()==testList[i+1].getStarRating())
            {
                assertTrue("Error: hotels are in wrong alphabetical order: " + i + " and " + (i+1), testList[i].getPropertyName().compareToIgnoreCase(testList[i+1].getPropertyName()) < 0);
            }
        }
        assertEquals("Error: hotel isn't from Manali", city, testList[testList.length-1].getCity());
    }

    @Test
    public void testByProximity1()
    {
        Hotel[] testList = site2.getHotelsByProximity(longitude, latitude);
        assertEquals("Error: did not return an empty array", 0, testList.length);
    }

    @Test
    public void testByProximity2()
    {
        ProximityComparator proxComp = new ProximityComparator(latitude, longitude);
        Hotel[] testList = site1.getHotelsByProximity(longitude, latitude);
        for(int i=0; i<testList.length-1; i++)
        {
            assertTrue("Error: hotels are in wrong order: " + i + " and " + (i+1), proxComp.getDistance(testList[i])<=proxComp.getDistance(testList[i+1]));
            if(proxComp.getDistance(testList[i])==proxComp.getDistance(testList[i+1]))
            {
                assertTrue("Error: hotels are in wrong POI order: " + i + " and " + (i+1), testList[i].getNumPOI() >= testList[i+1].getNumPOI());
            }
        }
    }

    @Test
    public void testByCityProximity1()
    {
        Hotel[] testList = site2.getHotelsInCityByProximity(city, longitude, latitude);
        assertEquals("Error: did not return an empty array", 0, testList.length);
    }

    @Test
    public void testByCityProximity2()
    {
        ProximityComparator proxComp = new ProximityComparator(latitude, longitude);
        Hotel[] testList = site1.getHotelsInCityByProximity(city, longitude, latitude);
        for(int i=0; i<testList.length-1; i++)
        {
            assertEquals("Error: hotel isn't from Manali", city, testList[i].getCity());
            assertTrue("Error: hotels are in wrong order", proxComp.compare(testList[i], testList[i+1])<=0);
            if(proxComp.getDistance(testList[i])==proxComp.getDistance(testList[i+1]))
            {
                assertTrue("Error: hotels are in wrong POI order", testList[i].getNumPOI() >= testList[i+1].getNumPOI());
            }
        }
        assertEquals("Error: hotel isn't from Manali", city, testList[testList.length-1].getCity());
    }
}
