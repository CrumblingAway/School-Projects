package SpaceWars;
import oop.ex2.*;

/**
 * The class responsible for reading the command line arguments and
 * creating the appropriate ships.
 */
public class SpaceShipFactory {
    public static SpaceShip[] createSpaceShips(String[] args) {
        SpaceShip[] shipArray = new SpaceShip[args.length];
        for(int i=0; i<args.length; i++)
        {
            switch(args[i])
            {
                case "h":
                    shipArray[i] = new HumanShip();
                    break;
                case "r":
                    shipArray[i] = new RunnerShip();
                    break;
                case "b":
                    shipArray[i] = new BasherShip();
                    break;
                case "a":
                    shipArray[i] = new AggressiveShip();
                    break;
                case "d":
                    shipArray[i] = new DrunkardShip();
                    break;
                case "s":
                    shipArray[i] = new RainManShip();
                    break;
            }
        }
        return shipArray;
    }
}
