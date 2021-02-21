#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LINE_MAX_SIZE 1024
#define SAMPLE_MAX_SIZE 100

#define INPUT_DELIMS " \t\r\n"
#define VALID_CHARS "0123456789."
#define VALID_DIGS "0123456789"
#define EPSILON 0.00000001

#define AFFINE(t, x) (t + floor((double)x) / 2)

/**
 * Adjust all values in array to be in range [0, 1]
 * @param arr array of values for normalization
 */
void normalize(double arr[], int size)
{
    double sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }

    /* If sum < 0.00000001 then it's considered 0 so we can't divide by it */
    if(sum < EPSILON)
    {
        return;
    }

    for (int j = 0; j < size; j++)
    {
        arr[j] /= sum;
    }
}

/**
 * Moves array values to the center by floor value (i.e. amount of
 * zeros to the left is equal to or smaller by 1 than amount of
 * zeros to the right)
 * @param arr array to be centralized
 */
void centralize(double arr[])
{
    int margin = 0; //Amount of empty indices
    int lastPos = 0; //Index of last positive number

    /* Check for zeros, starting from end
     * Because thanks to arrayParser "empty" indices are guaranteed to
     * be at the tail end of the array, which is iniated with 0.0's*/
    for (int i = SAMPLE_MAX_SIZE - 1; i >= 0; i--)
    {
        double num = arr[i];
        if (num == 0.0)
        {
            margin++;
        }
        else //Reached first non-zero member from end, save its index
        {
            lastPos = i;
            break;
        }
    }

    int move = margin / 2; //Move will get the floor value of the result in case of fraction
    if (move == 0)
    {
        return;
    }

    /* Use lastPos to move elements in array 'move' spaces to the left
     * starting from last element and nullify old cell. This way we avoid
     * changing cells we don't want.*/
    for (int j = lastPos; j >= 0; j--)
    {
        arr[j + move] = arr[j];
        arr[j] = 0;
    }
}

/**
 * Check if char c is in string chars
 * @param c char to validate
 * @param chars array of valid chars
 * @param size size of chars array
 * @return 0 if invalid, otherwise 1
 */
int isValidChar(char c, const char *chars, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (c == chars[i])
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Check if given number is integer greater than or equal to 0
 * @param arr char array representig number
 * @return 0 if not, otherwise 1
 */
int validPosInt(char *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (!isValidChar(arr[i], VALID_DIGS, strlen(VALID_DIGS)))
        {
            return 0;
        }
    }

    /* Number can't begin with 0 */
    if ((short) strlen(arr) > 1 && arr[0] == '0')
    {
        return 0;
    }

    return 1;
}

/**
 * Checks for valid decimal point amount and placement, and if string
 * actually represents a number
 * @param item string to be checked
 * @return 0 if not a number, otherwise 1
 */
int isValidNum(char *item)
{
    size_t length = strlen(item); //size of string
    /* Number has to be at most 9 chars long */
    if (length > 9)
    {
        return 0;
    }

    int dec_counter = 0; //counts amount of decimal points
    int dec_idx = 0; //index of decimal point, for future use (inside this function)
    for (int i = 0; i < (short) length; i++)
    {
        /* Check if number is not negative or starts with a decimal point */
        if (i == 0 && (item[i] == '-' || item[i] == '.'))
        {
            return 0;
        }

        /* Check if item consists only of "[0-9]." and count decimal points */
        if (!isValidChar(item[i], VALID_CHARS, strlen(VALID_CHARS)))
        {
            return 0;
        }
        else
        {
            if (item[i] == '.')
            {
                dec_counter++;
                if (dec_counter == 1) //Keep first appearance of decimal point
                {
                    dec_idx = i;
                }
                else if (dec_counter > 1) //More than one decimal point, invalid
                {
                    return 0;
                }
            }
        }

        /* Number ends with decimal point */
        if (i == length - 1 && item[i] == '.')
        {
            return 0;
        }
    }

    /* If decimal point has more than one number before it or there's
     * no decimal point*/
    if (dec_idx != 1)
    {
        /* If the input is just '0' */
        if (length == 1 && item[0] == '0')
        {
            return 1;
        }

        if (item[0] == '0')
        {
            return 0;
        }
    }

    return 1;
}

/**
 * Parses a string to a double, assumes item is valid input
 * @param item string representing double
 * @return double represented by string
 */
double parseToDouble(const char *item, int size)
{
    double result = 0;
    int dec_idx = 1; //placement of decimal point, if present

    /* Search for decimal point */
    for (int i = 0; i < size; i++)
    {
        if (item[i] == '.')
        {
            dec_idx = i;
            break;
        }
    }

    /* Add chars from item to result using powers of 10 */
    for (int j = 0; j < size; j++)
    {
        if (item[j] == '.')
        {
            continue;
        }
        int digit = item[j] - '0'; //convert char to int
        result += digit * pow(10, dec_idx - 1);
        dec_idx--;
    }

    return result;
}

/**
 * Parse a string to an int, assumes item is valid input
 * @param item string representing int
 * @return int represented by string
 */
int parseToInt(const char *item, int size)
{
    int result = 0;

    for (int i = 0; i < size; i++)
    {
        result += (int) ((item[i] - '0') * pow(10.0, size - (i + 1)));
    }

    return result;
}

/**
 * Parses array of doubles and keeps track of amount of elements
 * in user input
 * @param result array that will store the user input
 * @param inputAmount amount of elements in user input
 * @return EXIT_FAILURE if input is invalid, otherwise EXIT_SUCCESS
 */
int arrayParser(double *result, int* inputAmount)
{
    /* Initializing the variables that'll contain the line information */
    char line[LINE_MAX_SIZE];
    char *value = NULL;

    /* Reads the line from the stdin */
    if (fgets(line, LINE_MAX_SIZE, stdin) == NULL)
    {
        fprintf(stderr, "ERROR\n");
        return EXIT_FAILURE;
    }

    /* Read first value */
    value = strtok(line, INPUT_DELIMS);

    /* Check if value is a valid number, parse it, keep track of amount of
     * parsed values, make sure not to exceed 100 elements*/
    int idx = 0; //placement of parsed value in result
    while (value != NULL)
    {
        (*inputAmount)++;

        /* Check if we've exceeded permitted sample size */
        if (idx > SAMPLE_MAX_SIZE - 1)
        {
            fprintf(stderr, "ERROR\n");
            return EXIT_FAILURE;
        }

        /* Check if value is a valid number */
        if (!isValidNum(value))
        {
            fprintf(stderr, "ERROR\n");
            return EXIT_FAILURE;
        }

        result[idx] = parseToDouble(value, (short) strlen(value));
        idx++;

        value = strtok(NULL, INPUT_DELIMS);
    }

    return EXIT_SUCCESS;
}

/**
 * Parses user input to int
 * @param num number to store result
 * @return EXIT_FAILURE in case of error
 */
int intParser(int *num)
{
    /* Initializing the variables that'll contain the line information */
    char line[LINE_MAX_SIZE];
    char *value = NULL;

    /* Reads the line from the stdin */
    if (fgets(line, LINE_MAX_SIZE, stdin) == NULL)
    {
        fprintf(stderr, "ERROR\n");
        return EXIT_FAILURE;
    }

    /* Eliminate line break, but NOT whitespace */
    value = strtok(line, "\n");

    if (!validPosInt(value, (short) strlen(value)))
    {
        fprintf(stderr, "ERROR\n");
        return EXIT_FAILURE;
    }

    *num = parseToInt(value, (short) strlen(value));

    return EXIT_SUCCESS;
}

/**
 * One iteration of convolution
 * @param arr1 g array
 * @param size1 size of g
 * @param arr2 h array
 * @param size2 size of h
 * @param result array that will hold end result
 */
void littleConvolution(double arr1[], int size1, const double arr2[], int size2, double result[])
{
    double sum = 0.0;

    /* Half of array size */
    double half1 = (double) size1 / 2;
    double half2 = (double) size2 / 2;

    /* Start indices for t and m */
    int m_start = (int) ((-1) * (ceil(half2)));
    int t_start = (int) ((-1) * (ceil(half1))) - 1;

    /* Add results to array for every index t */
    for (int j = t_start; j < floor(half1) - 1; j++)
    {
        for (int i = m_start; i < floor(size2) + 1; i++)
        {
            int g_idx = AFFINE(j - i, size1);
            int h_idx = AFFINE(i, size2);

            /* If adjustsed indices are not within range */
            if ((g_idx < 0 || g_idx >= size1) ||
                (h_idx < 0 || h_idx >= size2))
            {
                continue;
            }
            else
            {
                sum += arr1[g_idx] * arr2[h_idx];
            }

        }

        result[j + (int) ceil(half1) + 1] = sum;
        sum = 0.0;
    }

    normalize(result, SAMPLE_MAX_SIZE);

    /* Copy contents of result to arr1 */
    for (int k = 0; k < size1; k++)
    {
        arr1[k] = result[k];
    }
}

/**
 * Performs convolution on two arrays n times
 * @param g g array
 * @param g_size size of g
 * @param h h array
 * @param h_size size of h
 * @param n amount of times to perform convolution (rooms)
 * @param result array that will hold end result
 */
void convolution(double g[], int g_size, double h[], int h_size, int n, double result[])
{
    /* If n is 0 don't calculate convolution and just return g */
    if (n == 0)
    {
        for (int i = 0; i < g_size; i++)
        {
            result[i] = g[i];
        }
    }

    for (int i = 0; i < n; i++)
    {
        littleConvolution(g, g_size, h, h_size, result);
    }
}

/**
 * Rounds every element in array to a thousandth and returns max element (before rounding)
 * @param arr array of elements to round
 * @param size size of array
 * @return max value from array before rounding
 */
double roundMax(double arr[], int size)
{
    double max = 0;

    /* Round every value to a thousandth and keep max */
    for (int i = 0; i < size; i++)
    {
        arr[i] = (round(1000 * arr[i])) / 1000;
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }

    return max;
}

/**
 * Prints all results with appropriate amount of stars in graph form
 * @param arr array of results
 * @param size size of arr
 */
void histogram(double arr[], int size)
{
    double max = roundMax(arr, size);

    /* Max has to be greater than epsilon to avoid division by zero */
    if (max < EPSILON)
    {
        return;
    }

    /* Calculate amount of stars for each result */
    int stars[SAMPLE_MAX_SIZE] = {0};
    for (int i = 0; i < size; i++)
    {
        stars[i] = floor(20 * arr[i] / max);
    }

    /* Print results with stars */
    for (int j = 0; j < size; j++)
    {
        printf("%.3lf: ", arr[j]);
        for (int k = 0; k < stars[j]; k++)
        {
            printf("*");
        }
        printf("\n");
    }
}

/**
 * Main function tht runs the entire program
 * @return
 */
int main()
{
    /* Instantiate both sample arrays */
    double drum_arr[SAMPLE_MAX_SIZE] = {0.0};
    double amp_arr[SAMPLE_MAX_SIZE] = {0.0};

    /* Counters for input given for both arrays */
    int drumCounter = 0;
    int ampCounter = 0;

    /* Parse items from user input into arrays above */
    if (arrayParser(drum_arr, &drumCounter) == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    if (arrayParser(amp_arr, &ampCounter) == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    /* Compare sample lengths (amp smaller than or equal to drum) */
    if (drumCounter < ampCounter)
    {
        fprintf(stderr, "ERROR\n");
        exit(EXIT_FAILURE);
    }

    /* Take care of room input */
    int n = 0;
    int *p_n = &n;

    if (intParser(p_n) == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }

    /* Centralize arrays */
    centralize(drum_arr);
    centralize(amp_arr);

    /* Normalize arrays */
    normalize(drum_arr, SAMPLE_MAX_SIZE);
    normalize(amp_arr, SAMPLE_MAX_SIZE);

    /* Store results of convolution in this array */
    double result[SAMPLE_MAX_SIZE] = {0.0};
    convolution(drum_arr, SAMPLE_MAX_SIZE, amp_arr, SAMPLE_MAX_SIZE, n, result);

    /* Finally print graph representation of convolution */
    histogram(result, SAMPLE_MAX_SIZE);
}