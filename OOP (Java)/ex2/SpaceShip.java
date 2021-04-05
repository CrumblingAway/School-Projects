package SpaceWars;

import java.awt.Image;
import oop.ex2.*;

/**
 * The API spaceships need to implement for the SpaceWars game. 
 * It is your decision whether SpaceShip.java will be an interface, an abstract class,
 *  a base class for the other spaceships or any other option you will choose.
 *  
 * @author oop
 */
public class SpaceShip{

    /** The spaceship's physics object*/
    SpaceShipPhysics ownPhysics = new SpaceShipPhysics();

    /** The spaceship's maximum energy level*/
    int maxEnergyLevel = 210;

    /** The spaceship's current energy level*/
    int currentEnergyLevel = 190;

    /** The spaceship's health*/
    int health = 22;

    /** The spaceship's shield: on/off*/
    boolean shield = false;

    /** Keeps count of shot cooldown*/
    int shotTimer = 0;

    /**
     * Does the actions of this ship for this round. 
     * This is called once per round by the SpaceWars game driver.
     * 
     * @param game the game object to which this ship belongs.
     */
    public void doAction(SpaceWars game)
    {

    }

    /**
     * This method is called every time a collision with this ship occurs 
     */
    public void collidedWithAnotherShip()
    {
        if(this.shield)
        {
            this.maxEnergyLevel += 18;
            this.currentEnergyLevel += 18;
        }
        else
        {
            this.health--;
        }
    }

    /** 
     * This method is called whenever a ship has died. It resets the ship's 
     * attributes, and starts it at a new random position.
     */
    public void reset()
    {
        this.ownPhysics = new SpaceShipPhysics();
        this.maxEnergyLevel = 210;
        this.currentEnergyLevel = 190;
        this.health = 22;
        this.shield = false;
        this.shotTimer = 0;
    }

    /**
     * Checks if this ship is dead.
     * 
     * @return true if the ship is dead. false otherwise.
     */
    public boolean isDead()
    {
        if(this.health <= 0)
        {
            return true;
        }
        return false;
    }

    /**
     * Gets the physics object that controls this ship.
     * 
     * @return the physics object that controls the ship.
     */
    public SpaceShipPhysics getPhysics()
    {
        return this.ownPhysics;
    }

    /**
     * This method is called by the SpaceWars game object when ever this ship
     * gets hit by a shot.
     */
    public void gotHit()
    {
        if(!this.shield)
        {
            this.health--;
            this.maxEnergyLevel -= 10;
            if(this.currentEnergyLevel < this.maxEnergyLevel)
            {
                this.currentEnergyLevel = this.maxEnergyLevel;
            }
        }
    }

    /**
     * Gets the image of this ship. This method should return the image of the
     * ship with or without the shield. This will be displayed on the GUI at
     * the end of the round.
     * 
     * @return the image of this ship.
     */
    public Image getImage()
    {
        if(this.shield)
        {
            return GameGUI.ENEMY_SPACESHIP_IMAGE_SHIELD;
        }
        return GameGUI.ENEMY_SPACESHIP_IMAGE;
    }

    /**
     * Attempts to fire a shot.
     * 
     * @param game the game object.
     */
    public void fire(SpaceWars game)
    {
        if(this.currentEnergyLevel >= 19 && this.shotTimer == 0)
        {
            game.addShot(this.ownPhysics);
            this.currentEnergyLevel -= 19;
            this.shotTimer = 7;
        }
    }

    /**
     * Attempts to turn on the shield.
     */
    public void shieldOn()
    {
        if(this.currentEnergyLevel >= 3)
        {
            this.shield = true;
            this.currentEnergyLevel -= 3;
        }
    }

    /**
     * Attempts to teleport.
     */
    public void teleport()
    {
       if(this.currentEnergyLevel >= 140)
       {
           this.ownPhysics = new SpaceShipPhysics();
           this.currentEnergyLevel -= 140;
       }
    }
}
