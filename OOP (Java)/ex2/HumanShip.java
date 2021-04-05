package SpaceWars;
import java.awt.Image;
import oop.ex2.*;

/**
 * The class for the ship controlled by the player.
 * Uses the user controlled spaceship image and checks for button presses.
 */
public class HumanShip extends SpaceShip
{
    /**
     * Gets the image of the user ship.
     * */
	public Image getImage()
    {
        if(this.shield)
        {
            return GameGUI.SPACESHIP_IMAGE_SHIELD;
        }
        return GameGUI.SPACESHIP_IMAGE;
    }

    /**
     * Does the actions of the ship for the round.
     * Only in this class does this method check for user input.
     * @param game
     */
    public void doAction(SpaceWars game)
    {
        // Shields are off by default.
    	this.shield = false;

    	// The shot timer has to count down if it's not zero, and doesn't
        // need to go any lower than zero.
        if(this.shotTimer > 0)
        {
            this.shotTimer -= 1;
        }

        // Teleport if pressed
        if(game.getGUI().isTeleportPressed())
        {
            teleport();
        }

        // Accelerate if pressed
        if(game.getGUI().isUpPressed())
        {
            this.ownPhysics.move(true, 0);
        }

        // Don't turn if left and right pressed together
        if(game.getGUI().isLeftPressed() && game.getGUI().isRightPressed())
        {
            this.ownPhysics.move(false, 0);
        }

        // Turn accordingly
        if(game.getGUI().isLeftPressed())
        {
            this.ownPhysics.move(false, 1);
        }

        if(game.getGUI().isRightPressed())
        {
            this.ownPhysics.move(false, -1);
        }

        // Turn on shields
        if(game.getGUI().isShieldsPressed())
        {
            shieldOn();
        }

        // Shoot
        if(game.getGUI().isShotPressed())
        {
            fire(game);
        }

        // Regenerate one point of energy
        if(this.currentEnergyLevel < this.maxEnergyLevel)
        {
            this.currentEnergyLevel++;
        }
    }
}