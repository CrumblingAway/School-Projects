package SpaceWars;
/**
 * Class of basher ship. Seeks out closest ship and attempts to ram it.
 * When it gets close enough (range 0.19) it turns on its shield.
 */
public class BasherShip extends SpaceShip
{

    /**
     * Performs the actions of the ship for this round.
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {
    	this.shield = false;

    	// Turn according to the closest ship.
        int turnAngle = 0;
        if(this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics()) > 0)
        {
            turnAngle = 1;
        }
        else if(this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics()) < 0)
        {
            turnAngle = -1;
        }
        this.ownPhysics.move(true, turnAngle);

        // Turn on shield when cloes to another ship.
        if(this.ownPhysics.distanceFrom(game.getClosestShipTo(this).getPhysics()) <= 0.19)
        {
            shieldOn();
        }

        if(this.currentEnergyLevel < this.maxEnergyLevel)
        {
            this.currentEnergyLevel++;
        }
    }
}
