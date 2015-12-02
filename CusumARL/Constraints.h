#ifndef TCLAP_FUNCTIONCONSTRAINT_H
#define TCLAP_FUNCTIONCONSTRAINT_H

#include <string>
#include <vector>
#include <tclap/Constraint.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define HAVE_SSTREAM
#endif

#if defined(HAVE_SSTREAM)
#include <sstream>
#elif defined(HAVE_STRSTREAM)
#include <strstream>
#else
#error "Need a stringstream (sstream or strstream) to compile!"
#endif

namespace TCLAP {

	/**
	* A Constraint that constrains the Arg to only those values specified
	* in the constraint.
	*/
	template<class T>
	class GreaterThan0Constraint : public Constraint<T>
	{

	public:

		/**
		* Constructor.
		* \param f - function which encodes cusmom constraints on the value from
		* the command line and returns true or faulse
		* \param desc - A description of the function behaviour
		*/
		GreaterThan0Constraint(bool(*f)(T), std::string desc);

		/**
		* Virtual destructor.
		*/
		virtual ~GreaterThan0Constraint() {}

		/**
		* Returns a description of the Constraint.
		*/
		virtual std::string description() const;

		/**
		* Returns the short ID for the Constraint.
		*/
		virtual std::string shortID() const;

		/**
		* The method used to verify that the value parsed from the command
		* line meets the constraint.
		* \param value - The value that will be checked.
		*/
		virtual bool check(const T& value) const;

	protected:

		/**
		* The string used to describe the allowed values of this constraint.
		*/
		std::string _typeDesc;

		/**
		* internal copy of the function to be used to check the value from
		* the command line
		*/
		bool(*func)(T);

	};

	template<class T>
	GreaterThan0Constraint<T>::GreaterThan0Constraint(bool(*f)(T), std::string desc)
		: _typeDesc(desc)
	{
		unc = (*f);
	}

	template<class T>
	bool GreaterThan0Constraint<T>::check(const T& val) const
	{
		return func(val);
	}

	template<class T>
	std::string GreaterThan0Constraint<T>::shortID() const
	{
		return _typeDesc;
	}

	template<class T>
	std::string GreaterThan0Constraint<T>::description() const
	{
		return _typeDesc;
	}


} //namespace TCLAP
#endif 
