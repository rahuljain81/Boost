#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <iostream>
#include <ctime>

namespace sc = boost::statechart;

struct EvStartStop : sc::event< EvStartStop > {
	EvStartStop()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~EvStartStop()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
};
struct EvReset : sc::event< EvReset > {
	EvReset()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~EvReset()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
};

struct IElapsedTime
{
	  virtual double ElapsedTime() const = 0;
};

struct Active;
struct StopWatch : sc::state_machine< StopWatch, Active > 
{
	StopWatch()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~StopWatch()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	double ElapsedTime() const
	{
		std::cout << __FUNCTION__ <<"\t"<< __LINE__<< "\t";
		return state_cast< const IElapsedTime & >().ElapsedTime();
	}
};

struct Stopped;

// The simple_state class emplate accepts up to four parameters:
// - The third parameter specifies the inner initial state, if
//   there is one. Here, only Active has inner states, which is
//   why it needs to pass its inner initial state Stopped to its
//   base
// - The fourth parameter specifies whether and what kind of
//   history is kept

// Active is the outermost state and therefore needs to pass the
// state machine class it belongs to

struct Active : sc::simple_state< Active, StopWatch, Stopped > 
{
	public:
	Active() : elapsedTime_( 0.0 )
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~Active()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	typedef sc::transition< EvReset, Active > reactions;
	double ElapsedTime() const { 
		std::cout << __FUNCTION__ <<"\t"<< __LINE__<< "\t";
		return elapsedTime_; 
	}
	double & ElapsedTime() { 
		std::cout << __FUNCTION__ <<"\t"<< __LINE__<< "\t";
		return elapsedTime_; 
	}
	
	private:
	double elapsedTime_;
};

// Stopped and Running both specify Active as their Context,
// which makes them nested inside Active
struct Running : IElapsedTime, sc::simple_state< Running, Active > 
{
	public:
	Running() : startTime_( std::time( 0 ) )
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~Running()
	{ 
		context< Active >().ElapsedTime() = ElapsedTime();
		std::cout << __FUNCTION__<< std::endl; 
	}
	virtual double ElapsedTime() const
	{
		std::cout << __FUNCTION__ <<"\t"<< __LINE__<< "\t";
		std::cout << "Sleeping for 3 sec\n";
		sleep (3);
		return context< Active >().ElapsedTime() + std::difftime( std::time( 0 ), startTime_ );
	}
	typedef sc::transition< EvStartStop, Stopped > reactions;
	private:
	    std::time_t startTime_;
};
struct Stopped : IElapsedTime, sc::simple_state< Stopped, Active > 
{
	Stopped()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	~Stopped()
	{ 
		std::cout << __FUNCTION__<< std::endl;
	}
	typedef sc::transition< EvStartStop, Running > reactions;
	virtual double ElapsedTime() const
	{
		std::cout << __FUNCTION__ << "\t" << __LINE__<< "\t";
		return context< Active >().ElapsedTime();
	}
};

// Because the context of a state must be a complete type (i.e.
// not forward declared), a machine must be defined from
// "outside to inside". That is, we always start with the state
// machine, followed by outermost states, followed by the direct
// inner states of outermost states and so on. We can do so in a
// breadth-first or depth-first way or employ a mixture of the
// two.

int main()
{
	StopWatch myWatch;
	myWatch.initiate();
	std::cout<<"StopWatch Creation Done"<<std::endl;
	std::cout << myWatch.ElapsedTime() << "\n";
	std::cout<<"Event:->\n";
	myWatch.process_event( EvStartStop() );
	std::cout << myWatch.ElapsedTime() << "\n";
	std::cout<<"Event:->\n";
	myWatch.process_event( EvStartStop() );
	std::cout << myWatch.ElapsedTime() << "\n";
	std::cout<<"Event:->\n";
	myWatch.process_event( EvStartStop() );
	std::cout << myWatch.ElapsedTime() << "\n";
	std::cout<<"Event:->\n";
	myWatch.process_event( EvReset() );
	std::cout << myWatch.ElapsedTime() << "\n";
	std::cout<<"Program has finished\n";
	return 0;
}
