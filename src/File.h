#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#ifndef FILE_H
#define FILE_H



std::vector<std::string> fileToBuffer(const char * file) {

	std::vector<std::string> wordList;

	std::string input;

	std::ifstream inputFile;

	inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// open files
		inputFile.open(file);

		// read file's buffer contents into streams
		std::stringstream inputStream;
		inputStream << inputFile.rdbuf();

		inputFile.close();

		input = inputStream.str();

		inputStream.str("");
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::INPUT::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	

	//std::cout << input << std::endl;
	

	int i = 0;
	if (input.length() == 0) {
		return wordList;
	}
	char temp = input[0];
	std::string word;
	while (i<input.length()) {
		if ((input[i] == ' ') || (temp == '\n') || (input[i] == '=')) {
			//word += '\0';
			if(word.length()>0) wordList.push_back(word);
			word = "";
		} else {
			word += temp;
		}
		i++;
		temp = input[i];
	}
	
	return wordList;
}




#endif