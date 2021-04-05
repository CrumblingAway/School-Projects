package SpaceWars;
/**
 * Class of aggressive ship. Turns towards closest ship and
 * fires when in range.
 */
public class AggressiveShip extends SpaceShip
{
    /**
     * Performs the actions of the ship for this round.
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {
        int turnAngle = 0;
        double closestAngle = this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics());

        if(this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics()) > 0)
        {
            turnAngle = 1;
        }
        else if(this.ownPhysics.angleTo(game.getClosestShipTo(this).getPhysics()) < 0)
        {
            turnAngle = -1;
        }
        this.ownPhysics.move(true, turnAngle);

        if(closestAngle < 0.21*Math.PI)
        {
            fire(game);
        }

        if(this.shotTimer > 0) {
            this.shotTimer--;
        }
    }
}
