#ifndef ERENUMS_H
#define ERENUMS_H

namespace ER {
	enum class SortBy {
		TITLE,
		DISTANCE,
		START_YEAR,
		NONE
	};
	static constexpr const char *SortByStrings[] = {
		"Title",
		"Distance",
		"Start Year",
		"None"
	};
}

#endif