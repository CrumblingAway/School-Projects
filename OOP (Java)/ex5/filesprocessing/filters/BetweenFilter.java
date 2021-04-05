package filesprocessing.filters;

import java.io.File;

/**
 * File size is between the two given numbers of bytes
 */
public class BetweenFilter implements Filter
{
    //Magic number
    private final double RATIO = 1024.0;

    //Fields
    private double minVal;
    private double maxVal;

    /**
     * Compare file size to size range
     * @param file file
     * @return true if size within range, otherwise false
     */
    @Override
    public boolean filter(File file)
    {
        return file.length()/RATIO >= minVal && file.length()/RATIO <= maxVal;
    }

    public BetweenFilter(double min, double max)
    {
        minVal = min;
        maxVal = max;
    }
}
