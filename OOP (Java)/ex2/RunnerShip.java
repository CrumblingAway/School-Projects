package SpaceWars;
/**
 * Class of runner ship. Tries to avoid conflict at all costs.
 * Runs away from closest ship, and if it gets too close, it teleports.
 */
public class RunnerShip extends SpaceShip
{

    /**
     * Performs the actions of the ship for this round.
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {
        // Check if distance from closest ship <0.25 and its angle to runner <0.23 radians
        if(this.ownPhysics.distanceFrom(game.getClosestShipTo(this).getPhysics()) < 0.25 &&
                game.getClosestShipTo(this).getPhysics().angleTo(this.getPhysics()) < 0.23*Math.PI)
        {
            teleport();
        }

        // Calculating angle to closest ship and setting turn to 1 or -1 accordingly
        int runAwayAngle = 0;
        double currentAngle = this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics());
        if(currentAngle < 0)
        {
            runAwayAngle = 1;
        }
        else if(currentAngle > 0)
        {
            runAwayAngle = -1;
        }

        this.ownPhysics.move(true, runAwayAngle);

        if(this.currentEnergyLevel < this.maxEnergyLevel)
        {
            this.currentEnergyLevel++;
        }
    }
}
