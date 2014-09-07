#pragma once

#include <fstream>
#include <time.h>
#include <string>

class BoltLog {

public:
	//If you can´t / dont - want - to use C++11, remove the "class" word after enum
	enum TYPE { LOG_ERROR, LOG_WARNING, LOG_INFO };

	// ctor (remove parameters if you don´t need them)
	explicit BoltLog(const char *fname = "c:/bolt_log.log")
		: numWarnings(0U),
		numErrors(0U)
	{

		myFile.open(fname, std::ios_base::app | std::ios_base::out);

		// Write the first lines
		//if (myFile.is_open()) {
		//	myFile << "My Game Engine, version " << std::endl;
		//	myFile << "Log file created" << std::endl << std::endl;
		//} // if

	}


	// dtor
	~BoltLog() {

		if (myFile.is_open()) {
			//myFile << std::endl << std::endl;

			// Report number of errors and warnings
			//myFile << numWarnings << " warnings" << std::endl;
			//myFile << numErrors << " errors" << std::endl;

			myFile.close();
		} // if

	}


	// Overload << operator using log type
	friend BoltLog &operator << (BoltLog &logger, const BoltLog::TYPE l_type){

		switch (l_type) {
		case BoltLog::LOG_ERROR:
			logger.myFile << "[ " << logger.currentDateTime() << " ] " << "[ERROR]: ";
			++logger.numErrors;
			break;

		case BoltLog::LOG_WARNING:
			logger.myFile << "[ " << logger.currentDateTime() << " ] " << "[WARNING]: ";
			++logger.numWarnings;
			break;

		default:
			logger.myFile << "[ " << logger.currentDateTime() << " ] " << "[INFO]: ";
			break;
		} // sw


		return logger;

	}


	// Overload << operator using C style strings
	// No need for std::string objects here
	friend BoltLog &operator << (BoltLog &logger, const char *text) {

		logger.myFile << text << std::endl;
		return logger;

	}

	friend BoltLog &operator << (BoltLog &logger, const std::string text) {

		logger.myFile << text << std::endl;
		return logger;

	}

	friend BoltLog &operator << (BoltLog &logger, const int text) {

		logger.myFile << text << std::endl;
		return logger;

	}


	// Make it Non Copyable (or you can inherit from sf::NonCopyable if you want)
	BoltLog(const BoltLog &) = delete;
	BoltLog &operator= (const BoltLog &) = delete;



private:
	std::string currentDateTime()
	{
		time_t     now = time(nullptr);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
		// for more information about date/time format
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		return buf;
	}

	std::ofstream           myFile;
	unsigned int            numWarnings;
	unsigned int            numErrors;

}; // class end