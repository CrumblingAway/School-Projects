#include <iostream>
#include "HashMap.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

#define VALID_ARGS_AMT 4

using std::pair;
using std::string;

/**
 * Check if char is '\n', '\t' or '\r'
 * @param letter char to check
 * @return true if one of the above, otherwise false
 */
bool isNTR(char letter)
{
	return (letter == '\n' || letter == '\t' || letter == '\r');
}

/**
 * Converts a string to be entirely in lower case
 * @param str string to convert
 * @return lower case version of string
 */
string lowerCase(const string &str)
{
	string result;
	
	for(char letter: str)
	{
		if(letter >= 'A' && letter <= 'Z')
		{
			result += (letter + 32);
		}
		else
		{
			result += letter;
		}
	}
	
	return result;
}

/**
 * Check if line in database is valid
 * @param file given file
 * @param line line in file
 * @return true if valid, otherwise false
 */
bool isValidDatabaseLine(std::fstream &file, string line)
{
	/* Valid line will have one comma */
	int commaCounter = 0;
	unsigned long commaIdx = 0;
	
	for(unsigned long i = 0; i < line.length(); ++i)
	{
		if(line[i] == ',')
		{
			commaIdx = i;
			commaCounter++;
		}
	}
	
	/* Only one comma allowed, can't be first or last char */
	if(commaCounter != 1 || commaIdx == 0 || commaIdx == (line.length() - 1))
	{
		file.close();
		return false;
	}
	
	for(unsigned long i = commaIdx + 1; i < line.length(); ++i)
	{
		if(!(line[i] >= '0' && line[i] <= '9') && !isNTR(line[i]))
		{
			file.close();
			return false;
		}
	}
	
	return true;
}

/**
 * Check if supplied database of bad words is valid while parsing the data
 * @param fileName file containing data
 * @param words container to store words
 * @return true if parse successful, otherwise false
 */
bool isValidDatabase(const string &fileName, HashMap<string, int> &words)
{
	std::fstream file;
	file.open(fileName);
	
	string line;
	
	int counterD = 0;
	
	/* Check validity of every line then parse the data */
	while(std::getline(file, line))
	{
		++counterD;
		
		if(!isValidDatabaseLine(file, line))
		{
			std::cout << counterD << std::endl;
			return false;
		}
		
		std::istringstream temp(line);
		string token;
		string value1;
		int value2 = 0;
		int counter = 0;
		
		while(std::getline(temp, token, ','))
		{
			if(counter == 0)
			{
				value1 = token;
			}
			else if(counter == 1)
			{
				value2 = std::stoi(token);
			}
			
			counter++;
		}
		
		words.insert(lowerCase(value1), value2);
	}
	
	file.close();
	return true;
}

/**
 * Checks if file exists
 * @param fileName file to check
 * @return true if exists, otherwise false
 */
bool isValidFile(const string &fileName)
{
	std::fstream file;
	file.open(fileName);
	
	if(!file)
	{
		file.close();
		return false;
	}
	
	file.close();
	return true;
}

/**
 * Check if given argument is a valid threshold
 * @param arg argument to check
 * @return true if valid, otherwise false
 */
bool isValidThreshold(string &arg)
{
	for(char letter: arg)
	{
		if(!(letter >= '0' && letter <= '9'))
		{
			return false;
		}
	}
	
	return std::stoi(arg) > 0;
}

/**
 * Checks if supplied arguments are valid
 * @param argc amount of arguments supplied
 * @param argv actual arguments
 * @param words container to store bad words with their score
 * @return true if valid, otherwise false
 */
int areValidArgs(int argc, char **argv, HashMap<string, int> &words)
{
	if(argc != VALID_ARGS_AMT)
	{
		std::cerr << "Usage: SpamDetector <database path> <message path> <threshold>\n";
		return EXIT_FAILURE;
	}
	
	string threshold(argv[3]);
	
	if(!isValidFile(argv[1]) || !isValidFile(argv[2]) || !isValidThreshold(threshold) || !isValidDatabase(argv[1], words))
	{
		std::cerr << "Invalid input\n";
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

/**
 * Check if given file is spam or not based on given bad words
 * @param fileName file to check for spam
 * @param words words that add to spam score
 * @param threshold threshold for spam score
 * @return true if spam, otherwise false
 */
bool isSpam(const string &fileName, HashMap<string, int> &words, int threshold)
{
	HashMap<string, int> myMap;
	
	std::ifstream file;
	file.open(fileName);
	string key;
	string temp; // String to read mail into
	int score = 0; // Spam score of mail
	
	/* Read file with two while loops. For every word in mail read from it
	 * to the end of the mail while adding every word and checking if the
	 * phrase appears in the database */
	while(!file.eof())
	{
		int location = file.tellg();
		while(!file.eof())
		{
			file >> temp;
			/* Line is empty */
			if(temp.length() == 0)
			{
				break;
			}
			
			temp = lowerCase(temp);
			
			/* Remove punctuation marks from start of word */
			if(temp[0] > 'z' || temp[0] < 'a')
			{
				temp = temp.substr(1);
			}
			
			/* Remove any punctuation marks from end of word */
			if(temp[temp.length() - 1] > 'z' || temp[temp.length() - 1] < 'a')
			{
				temp = temp.substr(0, temp.length() - 1);
			}
			
			key += (" " + temp);
			
			/* Remove unnecessary space added in the first iteration */
			if(key[0] > 'z' || key[0] < 'a')
			{
				key = key.substr(1);
			}
			
			if(words.containsKey(key))
			{
				score += words[key];
				myMap.insert(key, words[key]);
				key = "";
				break;
			}
		}
		
		key = "";
		file.clear();
		file.seekg(location, std::ios::beg);
		file >> temp;
	}
	
	file.close();
	
	return score >= threshold;
}

/**
 * Main function for running program
 * @return
 */
int main(int argc, char **argv)
{
	HashMap<string, int> words;

	if(areValidArgs(argc, argv, words) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if(isSpam(argv[2], words, std::stoi(argv[3])))
	{
		std::cout << "SPAM\n";
	}
	else
	{
		std::cout << "NOT_SPAM\n";
	}

	return EXIT_SUCCESS;
}