#ifndef NAVIGATION_H
#define NAVIGATION_H

namespace Navigation {
	enum Mode {
		FIRST_PERSON,
		FLIGHT,
		OBJECT
	};
	static constexpr const char *ModeStrings[] = {
		"First Person",
		"Flight",
		"Object"
	};
};

#endif