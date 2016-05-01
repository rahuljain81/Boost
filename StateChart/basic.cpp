#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/event.hpp>
#include <boost/mpl/list.hpp>

#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;
namespace sc = boost::statechart;
namespace mpl = boost::mpl;

//Defining States
struct state_1; //  state 1
struct state_2; //  state 2
struct state_3; //  state 3
struct state_4; //  state 4

//Defining Events
struct EvYes : sc::event< EvYes > {
	EvYes()
	{
		cout << "Event EvYes \n";
	}
};
struct EvNo : sc::event< EvNo > 
{
	EvNo()
	{
		cout << "Event EvNo\n";
	}
};
struct EvTimer : sc::event< EvTimer > {};//vyprsel cas

//Defining StateMachine
struct DU : sc::state_machine< DU, state_1 > {};
DU Zm;

void* casovac(void* s)
{
	int es = (intptr_t)s;
	sleep (es);
	Zm.process_event (EvTimer());
	return NULL;
}

struct state_1 : sc::simple_state< state_1, DU> // stav
{
	state_1() { cout<<"State 1 a-> State4 n->State2 (a/n) ? \n";}//FSM_Entry
	~state_1() {}//FSM_Exit
	typedef mpl::list< // reakce na udalosti
		sc::transition< EvYes, state_4 >,
		sc::transition< EvNo, state_2 > > reactions;
};
struct state_2 : sc::simple_state< state_2, DU >
{
	state_2()
	{
		cout<<"State 2 Entered \n";
		pthread_t idthread;
		int *a = (int* )3;
		if ( pthread_create(&idthread, NULL, casovac, a) != 0)
		{
			cout << "%s: Thread Created \n"<<__func__;
			abort();
		}
		if ( pthread_detach(idthread ) )
		{
			cout << "State 2: Error detaching ..\n";
			abort ();
		}
	}
	~state_2() {}
	typedef sc::transition< EvTimer, state_3 > reactions;
};

struct state_3 : sc::simple_state< state_3, DU >
{
	pthread_t idthread;
	state_3() { 
		cout<<"State 3 Entered \n";
		int *a = (int* ) 1;
		if ( pthread_create(&idthread, NULL, casovac, a) != 0)
		{
			cout << "Thread Create failed\n.";
			abort();
		}
		if ( pthread_detach(idthread ) )
		{
			cout << "State 3: Error detaching ..\n";
			abort ();
		}
	}
	~state_3() {}
	typedef mpl::list<
		sc::transition< EvNo, state_1 >,
		sc::transition< EvTimer, state_4 > > reactions;
};
struct state_4 : sc::simple_state< state_4, DU >
{
	state_4() {
		cout<<"State 4 Entered \n";
		pthread_t idthread;
		int *a = (int* )2;
		if ( pthread_create(&idthread, NULL, casovac, a) != 0)
		{
			cout << "Thread Create failed\n.";
			abort();
		}
		if ( pthread_detach(idthread ) )
		{
			cout << "State 4: Error detaching ..\n";
			abort ();
		}
	}
	~state_4() { cout<<"HOTOVO.\n";}
	typedef sc::transition< EvTimer, state_1 > reactions;
};

void *nacitani (void * ptr)
{
	string s = "";
	while(1)
	{
		cin >> s;
		if (s == "n") Zm.process_event (EvNo());
		if (s == "a") Zm.process_event (EvYes());
		if (s == "exit") break;
	}
	return NULL;
}
int main()
{
	Zm.initiate();
	pthread_t idthread;
	if ( pthread_create(&idthread, NULL, nacitani, NULL) != 0)
	{
		cout << "Thread Create failed\n.";
		abort();
	}
	pthread_join(idthread, NULL);
	return 0;
}
