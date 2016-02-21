/*
 * Copyright (c) 2016 Julien "Derjik" Laurent <julien.laurent@engineer.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../include/Logger.hpp"
#include <iomanip>
#include <sstream>


namespace Mach
{

using namespace std;


/*
 * Some default priorities (index & string tag)
 */
Priority LOG_DEBUG = { 0, "DEBUG" };
Priority LOG_INFO = { 1, "INFO" };
Priority LOG_WARN = { 2, "WARN" };
Priority LOG_ERROR = { 3, "ERROR" };

/*
 * This constructor tries to open the given file, and sets the SubLoggers to
 * their adequate priorities
 */
Logger::Logger(string const & pathToLogFile, Priority const priority) :
	_logFile(pathToLogFile, ios::trunc | ios::out),
	_minimumPriority(priority),
	_newLine(true),
	debug(this, LOG_DEBUG),
	info(this, LOG_INFO),
	warn(this, LOG_WARN),
	error(this, LOG_ERROR)
{}

/*
 * Destructor (closes the log file)
 */
Logger::~Logger()
{
	_logFile.close();
}

/*
 * Insert the logging prefix at the beginning of a new log line
 */
void Logger::prefix(Priority const priority)
{
	if(_newLine)
	{
		/* Add the formatted timestamp and a space */
		_logFile << formattedLogTime() << " ";

		/* Add the Priority prefix */
		_logFile << "[" << priority << "] ";

		/* Set the newLine flag off */
		_newLine = false;
	}
}

/*
 * Get a formatted timestamp
 */
string Logger::formattedLogTime()
{
	/* Get a raw time value */
	time_t rawtime(time(nullptr));
	/* Convert it into a "tm" structure */
	tm const * timeinfo(localtime(&rawtime));
	/* Prepare the output string stream */
	ostringstream s;

	/* Insert data */
	s << "[" << setw(2) << setfill('0') << timeinfo->tm_hour;
	s << ":" << setw(2) << setfill('0') << timeinfo->tm_min;
	s << ":" << setw(2) << setfill('0') << timeinfo->tm_sec;
	s << "]";

	/* Stringify! */
	return s.str();
}

/*
 * Special output operator used along with gadgets like our custom endl
 */
Logger::Level & Logger::Level::operator <<
(Logger::Level & (*f) (Logger::Level &))
{
	/* Apply f() to the current Logger */
	return f(*this);
	/* Note: this construct is strongly inspired from
	 * the std::basic_ostream & std::endl approach,
	 * giving transparency to the subsequent calls */
}

/*
 * Custom endl used with Level to end an open log line
 */
Logger::Level & endl(Logger::Level & l)
{
	/* Add the newline character */
	l._logger->_logFile << std::endl;
	/* Set the newline flag on */
	l._logger->_newLine = true;

	return l;
}

/*
 * Sets the minimum priority to the given one, thus changing the actual logging
 * verbosity.
 */
void Logger::setVerbosity(Priority const priority)
{
	_minimumPriority = priority;
}

/*
 * The Level constructor takes a master logger pointer and a current logging
 * priority for arguments: the master logger will receive the data if the
 * Level's priority is high enough.
 */
Logger::Level::Level(Logger * const logger, Priority const priority) :
	_logger(logger),
	_priority(priority)
{}

/*
 * Destructor
 */
Logger::Level::~Level()
{}

}
