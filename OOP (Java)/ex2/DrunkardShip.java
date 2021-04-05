package SpaceWars;
/**
 * Class of drunkard ship. Simply fires, turns and teleports
 * completely at random.
 */
public class DrunkardShip extends SpaceShip
{
    /**
     * Performs the actions of the ship for this round.
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {
        if(this.shotTimer > 0)
        {
            this.shotTimer -= 1;
        }

        // Random turning
        int turnAngle = (int) (Math.random()*4 - 2);

        // Random shooting
        int shotChoice = (int) (Math.random()*51);

        // Random teleport

        int teleportVar = (int) (Math.random()*100000);

        if(teleportVar <= 50)
        {
            teleport();
        }

        this.ownPhysics.move(true, turnAngle);

        if(shotChoice >= 40)
        {
            fire(game);
        }

        if(this.currentEnergyLevel < this.maxEnergyLevel)
        {
            this.currentEnergyLevel = this.maxEnergyLevel;
        }
    }
}
