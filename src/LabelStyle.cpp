#include "LabelStyle.h"

LabelStyle::LabelStyle() :
	font("\"Arial\""), size(12), red(240), green(240), blue(240), opacity(255),
	shadow(false), red_bg(0), green_bg(0), blue_bg(0), opacity_bg(178), width(200), height(150),
	weight("regular"), align("left"), margin(10)
{

}

LabelStyle::LabelStyle(const LabelStyle& n, const osg::CopyOp& copyop) :
	font(n.font), size(n.size), red(n.red), green(n.green), blue(n.blue), opacity(n.opacity),
	shadow(n.shadow), red_bg(n.red_bg), green_bg(n.green_bg), blue_bg(n.blue_bg), opacity_bg(n.opacity_bg), 
	width(n.width), height(n.height), weight(n.weight), align(n.align), margin(n.margin)
{

}

LabelStyle::LabelStyle(const std::string f, int s, int r, int g, int b, float o, bool sh, int rbg,
	int bbg, int gbg, float obg, int w, int h, const std::string we, const std::string al, int m) :
	font(f), size(s), red(r), green(g), blue(b), opacity(o),
	shadow(sh), red_bg(rbg), green_bg(gbg), blue_bg(bbg), opacity_bg(obg), width(w), height(h),
	weight(we), align(al), margin(m)
{

}

LabelStyle::~LabelStyle()
{

}

LabelStyle& LabelStyle::operator=(const LabelStyle& other)
{
	if (this != &other)
	{
		font = other.font;
		size = other.size;
		red = other.red;
		green = other.green;
		blue = other.blue;
		opacity = other.opacity;
		shadow = other.shadow;
		red_bg = other.red_bg;
		blue_bg = other.blue_bg;
		green_bg = other.green_bg;
		opacity_bg = other.opacity_bg;
		width = other.width;
		height = other.height;
		weight = other.weight;
		align = other.align;
		margin = other.margin;
	}

	return *this;
}


const std::string& LabelStyle::getFont() const
{
	return font;
}
void LabelStyle::setFont(const std::string& f)
{
	font = f;
}
int LabelStyle::getSize() const
{
	return size;
}
void LabelStyle::setSize(int s)
{
	size = s;
}
int LabelStyle::getRed() const
{
	return red;
}
void LabelStyle::setRed(int r)
{
	red = r;
}
int LabelStyle::getBlue() const
{
	return blue;
}
void LabelStyle::setBlue(int b)
{
	blue = b;
}
int LabelStyle::getGreen() const
{
	return green;
}
void LabelStyle::setGreen(int g)
{
	green = g;
}
float LabelStyle::getOpacity() const
{
	return opacity;
}
void LabelStyle::setOpacity(float o)
{
	opacity = o;
}
bool LabelStyle::getShadow() const
{
	return shadow;
}
void LabelStyle::setShadow(bool s)
{
	shadow = s;
}

int LabelStyle::getRed_BG() const
{
	return red_bg;
}
void LabelStyle::setRed_BG(int rbg)
{
	red_bg = rbg;
}
int LabelStyle::getBlue_BG() const
{
	return blue_bg;
}
void LabelStyle::setBlue_BG(int bbg)
{
	blue_bg = bbg;
}
int LabelStyle::getGreen_BG() const
{
	return green_bg;
}
void LabelStyle::setGreen_BG(int gbg)
{
	green_bg = gbg;
}
float LabelStyle::getOpacity_BG() const
{
	return opacity_bg;
}
void LabelStyle::setOpacity_BG(float obg)
{
	opacity_bg = obg;
}
int LabelStyle::getWidth() const
{
	return width;
}
void LabelStyle::setWidth(int w)
{
	width = w;
}
int LabelStyle::getHeight() const
{
	return height;
}
void LabelStyle::setHeight(int h)
{
	height = h;
}

const std::string& LabelStyle::getWeight() const
{
	return weight;
}
void LabelStyle::setWeight(const std::string& Weight)
{
	weight = Weight;
}
const std::string& LabelStyle::getAlign() const
{
	return align;
}
void LabelStyle::setAlign(const std::string& Align)
{
	align = Align;
}

int LabelStyle::getMargin() const
{
	return margin;
}

void LabelStyle::setMargin(int m)
{
	margin = m;
}