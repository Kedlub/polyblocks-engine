#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

//Lot of these functions are proudly stolen from StackOverflow

bool stringContains(std::string text, char* contains) {
	return text.find(contains) != std::string::npos;
}

/*
Check if string is a number
*/
bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

/*
* Erase First Occurrence of given  substring from main string.
*/
void eraseSubStr(std::string& mainStr, const std::string& toErase)
{
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	//printf("Spliting string %s by char %s", txt, ch);
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		//printf("Splitted %s", txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

std::unordered_map<char, int> fixedChars({ {'�',236}, {'�', 154}, {'�', 232}, {'�', 248}, {'�', 158},
										{'�', 253}, {'�', 225}, {'�', 237}, {'�', 233}, {'�', 249}, {'�', 250} });

int getFixedCharID(char ch) {
	for (std::pair<char, int> pair : fixedChars) {
		if (pair.first == ch) {
			return pair.second;
		}
	}
	return +ch;
}

bool isLowerBoundGreaterThanUpper(std::vector<double> lowerBound, std::vector<double> upperBound) {
	for (unsigned int i = 0; i < 3; i++) {
  		// Validate the bound.
  		if (lowerBound[i] > upperBound[i]) {
        		return true;
    		}
	}
	return false;
}
