#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include <string>
#include <fstream>


namespace Mach
{

/*
 * Defines a basic logging priority, as found in many other logging facilities
 */
typedef struct Priority
{
	/* Used as a threshold when deciding whether to log or not */
	int level;

	/* Will appear as [tag] at the beginning of log lines */
	std::string tag;

	/* Logging priorities must be compared using their respective integer
	 * levels */
	bool operator >= (Priority const & p)
	{
		return level >= p.level;
	}
} Priority;

/*
 * Allows use of << on streams capable of receiving an unsigned integer
 */
template <typename T> T & operator << (T & obj, Priority const & prio)
{
	obj << prio.tag;

	return obj;
}

/* Default priorities for the Logger */
extern Priority LOG_DEBUG;	/* Execution details */
extern Priority LOG_INFO;	/* Notices worth reading */
extern Priority LOG_WARN;	/* Safe but abnormal behaviors */
extern Priority LOG_ERROR;	/* Fatal abnormal behaviors */

/*
 * Logging facility
 *
 * Takes a file path and a minimal priority in parameters, allowing to filter
 * and output messages to the destination logfile.
 */
class Logger
{
	private:
		/* Destination logfile */
		std::ofstream _logFile;

		/* Current minimum priority */
		Priority _minimumPriority;

		/* Are we beginning a new line ? */
		bool _newLine;

	protected:
		/* Get a formatted [HOUR:MIN:SEC] timestamp */
		static std::string formattedLogTime();

		/* Insert the message prefix */
		void prefix(Priority const);

	public:
		/* Internal logging level to be exposed to the user */
		class Level
		{
			private:
				/* Parent logger */
				Logger * _logger;

				/* Logging priority */
				Priority _priority;

			public:
				/* Constructors & destructor */
				Level(Logger * const, Priority const);
				virtual ~Level();

				/* Logging operator */
				template <typename T>
				Level & operator << (T const & object)
				{
					/* Check the Level's priority
					 * before logging anything */
					if(_priority
						>= _logger->_minimumPriority)
					{
						/* Add the prefix */
						_logger->prefix(_priority);

						/* Log the message */
						_logger->_logFile << object;
					}

					/* Return a reference to self (useful
					 * for chained calls) */
					return (*this);
				}

				Level & operator << (Level & (*f) (Level &));
				friend Logger::Level & endl(Logger::Level &);
		};

		/* Constructors & destructor */
		Logger(std::string const & pathToLogFile, Priority const);
		virtual ~Logger();

		/* Copy & assignation are forbidden */
		Logger(Logger const &) = delete;
		Logger & operator = (Logger const &) = delete;

		/* Change verbosity */
		void setVerbosity(Priority const);

		/* Logging levels */
		Level debug;
		Level info;
		Level warn;
		Level error;

		/* Our custom endl needs access to private members */
		friend Logger::Level & endl(Logger::Level &);
};

/* Custom endl */
Logger::Level & endl(Logger::Level & l);

}

#endif // LOGGER_HPP_INCLUDED
