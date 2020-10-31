#include "../include/Mach/Exception.hpp"


namespace Mach
{

using namespace std;


/*
 * The main constructor takes the error message for argument
 */
Exception::Exception(string const message) : _message(message)
{}

/*
 * Empty destructor
 */
Exception::~Exception()
{}

/*
 * Error message getter
 */
string Exception::message() const
{
	return _message;
}

}
