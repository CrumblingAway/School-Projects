package SpaceWars;
/**
 * Class of special ship. Just flies around in a circle and fires
 * endlessly.
 */
public class RainManShip extends SpaceShip
{

    /**
     * Performs the actions of the ship for this round.
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {
        this.ownPhysics.move(true, -1);

        // Timer always at 0 so can fire endlessly.
        this.shotTimer = 0;

        //Always have max energy to be able to fire.
        this.currentEnergyLevel = this.maxEnergyLevel;

        fire(game);
    }
}
