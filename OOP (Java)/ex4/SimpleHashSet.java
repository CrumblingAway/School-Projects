/**
 * A superclass for implementations of hash-sets implementing the SimpleSet
 * interface.
 */
public abstract class SimpleHashSet implements SimpleSet {
    //Final fields
    protected static final float DEFAULT_HIGHER_CAPACITY = 0.75f;
    protected static final float DEFAULT_LOWER_CAPACITY = 0.25f;
    protected static final int INITIAL_CAPACITY = 16;
    protected static final double SHRINK_FACTOR = 0.5;
    protected static final double GROW_FACTOR = 2;

    //Fields
    protected float highCap;
    protected float lowCap;
    protected int ownCap;
    protected int size;

    /**
     * @return The current capacity (number of cells) of the table.
     */
    public abstract int capacity();

    /**
     * Clamps hashing indices to fit within the current table capacity.
     *
     * @param index the index before clamping
     * @return an index properly clamped
     */
    protected int clamp(int index)
    {
        return index & (this.capacity()-1);
    }

    /**
     * @return The lower load factor of the table.
     */
    protected float getLowerLoadFactor()
    {
        return DEFAULT_LOWER_CAPACITY;
    }

    /**
     * @return The upper load factor of the table.
     */
    protected float getUpperLoadFactor()
    {
        return DEFAULT_HIGHER_CAPACITY;
    }

    //Constructors

    /**
     * Custom constructor
     * @param upperLoadFactor
     * @param lowerLoadFactor
     */
    protected SimpleHashSet(float upperLoadFactor, float lowerLoadFactor)
    {
        this.highCap = upperLoadFactor;
        this.lowCap = lowerLoadFactor;
        this.ownCap = INITIAL_CAPACITY;
    }

    /**
     * Default constructor
     */
    protected SimpleHashSet()
    {
        this.highCap = DEFAULT_HIGHER_CAPACITY;
        this.lowCap = DEFAULT_LOWER_CAPACITY;
        this.ownCap = INITIAL_CAPACITY;
    }
}
