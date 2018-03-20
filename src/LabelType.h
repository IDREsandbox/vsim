#ifndef LABELTYPE_H
#define LABELTYPE_H

enum LabelType {
	NONE,
	HEADER1,
	HEADER2,
	BODY,
	LABEL
};
static constexpr const char *LabelTypeNames[] = {
	"None",
	"Header 1",
	"Header 2",
	"Body",
	"Label"
};

#endif