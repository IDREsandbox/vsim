#ifndef LABELSTYLEGROUP_H
#define LABELSTYLEGROUP_H

#include <memory>
#include "Canvas/LabelType.h"

class FrameStyle;
class LabelStyle;

class LabelStyleGroup {
public:
	LabelStyleGroup();
	~LabelStyleGroup();

	void copy(const LabelStyleGroup &other);

	LabelStyle *getStyle(LabelType style) const;
	FrameStyle *getImageStyle() const;

private:
	std::unique_ptr<LabelStyle> m_h1;
	std::unique_ptr<LabelStyle> m_h2;
	std::unique_ptr<LabelStyle> m_bod;
	std::unique_ptr<LabelStyle> m_lab;
	std::unique_ptr<FrameStyle> m_image;
};

#endif // LABELSTYLEGROUP_H