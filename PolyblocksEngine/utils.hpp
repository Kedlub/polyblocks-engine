#ifndef UTILS_HPP
#define UTILS_HPP

bool stringContains(std::string text, char *contains);

void eraseSubStr(std::string &mainStr, const std::string &toErase);

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);

int getFixedCharID(char ch);

bool isLowerBoundGreaterThanUpper(std::vector<double> lowerBound, std::vector<double> upperBound);

#endif // !UTILS_HPP
