#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>
#include <iostream>

using namespace std;

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

struct EvPressButton : sc::event<EvPressButton>
{
	EvPressButton()
	{
		cout << "Button is pressed" << endl;
	}
};

struct EvReleaseButton : sc::event<EvReleaseButton>
{
	EvReleaseButton()
	{
		cout << "Button is released" << endl;
	}
};

struct EvToggleLight : sc::event<EvToggleLight> {};

struct Active;
struct Switch : sc::state_machine<Switch, Active> {};

struct ButtonReleased;
struct ButtonPressed;
struct LightOff;
struct LightOn;

struct Active: sc::simple_state<Active, Switch,mpl::list<ButtonReleased, LightOff> > {};
struct ButtonPressed : sc::state<ButtonPressed, Active::orthogonal<0> >
{
	typedef sc::transition<EvReleaseButton, ButtonReleased>
		reactions;
	ButtonPressed(my_context ctx) : my_base(ctx)
	{
		post_event(EvToggleLight());
	}
};

struct ButtonReleased : sc::simple_state<ButtonReleased,
	Active::orthogonal<0> >
{
	typedef sc::transition<EvPressButton, ButtonPressed> reactions;
};

struct LightOff : sc::simple_state<LightOff, Active::orthogonal<1> >
{
	typedef sc::transition<EvToggleLight, LightOn> reactions;
	LightOff()
	{
		cout << "Light is off" << endl;
	}
};

struct LightOn : sc::simple_state<LightOn, Active::orthogonal<1> >
{
	typedef sc::transition<EvToggleLight, LightOff> reactions;
	LightOn()
	{
		cout << "Light is on" << endl;
	}
};

int main()
{
	Switch sw;
	sw.initiate();
	for (int i = 0; i < 5; i++)
	{
		sw.process_event(EvPressButton());
		sw.process_event(EvReleaseButton());
	}
	return 0;
}

