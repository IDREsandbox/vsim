#include <algorithm>
#include <QDebug>
#include "Util.h"

#include <osg/Matrix>
#include <osg/io_utils>
#include <iostream>
#include <chrono>
#include <regex>
#include <unordered_set>
#include <unordered_map>

// ext must be of the form "txt".
std::string Util::addExtensionIfNotExist(const std::string& filename, const std::string& ext)
{
    size_t fs = filename.rfind("/");
    size_t bs = filename.rfind("\\");
    size_t s;
    if (fs == std::string::npos)
        s = bs;
    else
        s = fs;
    int n;
    if (s == std::string::npos)
        n = -1;
    else
        n = s;
    size_t d = filename.rfind('.');
    if (d == std::string::npos || (int)d < n)
        return filename + "." + ext;
    else
        return filename;
}

std::string Util::getExtension(const std::string& filename)
{
    size_t fs = filename.rfind("/");
    size_t bs = filename.rfind("\\");
    size_t s;
    if (fs == std::string::npos)
        s = bs;
    else
        s = fs;
    int n;
    if (s == std::string::npos)
        n = -1;
    else
        n = s;
    size_t d = filename.rfind('.');
    if (d == std::string::npos || (int)d < n)
        return "";
    else
    {
        std::string ext = filename.substr(d + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
}

std::string Util::getFilename(const std::string & path)
{
	int endpath = path.find_last_of("\\/");
	return path.substr(endpath + 1);
}

bool Util::mxdxyyToQDate(const std::string & str, QDate *out)
{
	std::regex reg("(\\d{1,2})/(\\d{1,2})/(\\d{2})");
	std::smatch match;
	bool hit = std::regex_match(str, match, reg);
	if (!hit || match.size() != 4) {
		return false;
	}
	int m, d, y;
	m = std::stoi(match[1].str());
	d = std::stoi(match[2].str());
	y = std::stoi(match[3].str());
	if (y < 80) {
		y = 2000 + y;
	}
	else {
		y = 1900 + y;
	}
	*out = QDate(y, m, d);
	return true;
}

static const std::regex g_node_time_regex(".*T:.*?(-?\\d+) (-?\\d+)");
bool Util::nodeTimeInName(const std::string & name, int * begin, int * end)
{
	//const std::regex g_node_time_regex(".*T:.*?(-?\\d+) (-?\\d+)");
	//std::regex r(".*T:.*?(-?\\d+)(-?\\d+)");
	std::smatch match;
	if (std::regex_match(name, match, g_node_time_regex)) {
		*begin = std::stoi(match[1]);
		*end = std::stoi(match[2]);
		return true;
	}
	return false;
}

QRect Util::rectFit(QRect container, float whratio)
{
	float container_ratio = container.width() / (float) container.height();
	
	int width, height;
	int top, left;

	// container is wider, so match the vertical
	if (container_ratio >= whratio) {
		height = container.height();
		width = whratio*height;
	
		int leftover = container.width() - width;
		top = 0;
		left = leftover / 2;
	}
	// container is taller, so match the horizontal
	else {
		width = container.width();
		height = (1 / whratio)*width;

		int leftover = container.height() - height;
		left = 0;
		top = leftover / 2;
	}

	return QRect(left, top, width, height);
}

bool Util::isDescendant(QWidget *widget, QWidget *child)
{
	while (child != nullptr) {
		if (widget == child) return true;
		child = child->parentWidget();
	}
	return false;
}

osg::Image *Util::imageQtToOsg(const QImage & qimg)
{
	QImage img_rgba8 = qimg.convertToFormat(QImage::Format_RGBA8888);
	osg::Image *oimg = new osg::Image;

	unsigned char *data = new unsigned char[img_rgba8.byteCount()];
	memcpy(data, img_rgba8.bits(), img_rgba8.byteCount());

	oimg->setImage(qimg.width(), qimg.height(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE, 1);
	return oimg;
}

QImage Util::imageOsgToQt(const osg::Image *oimg)
{
	// TODO: lots of possible bugs here
	// - wrong format
	// - non-contiguous rows
	// - WARNING: the QImage references the osg byte array instead of copying, this could be a problem
	if (oimg == nullptr) {
		qWarning() << "Error: trying to convert a null image";
		return QImage();
	}
	return QImage(oimg->data(), oimg->s(), oimg->t(), QImage::Format_RGBA8888);
}

QColor Util::vecToColor(const osg::Vec4 & vec)
{
	QColor color;
	color.setRgbF(vec.r(), vec.g(), vec.b(), vec.a());
	return color;
}

osg::Vec4 Util::colorToVec(const QColor & color)
{
	return osg::Vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

QString Util::colorToStylesheet(QColor color)
{
	QString style = "background:rgba("
		+ QString::number(color.red()) + ","
		+ QString::number(color.green()) + ","
		+ QString::number(color.blue()) + ","
		+ QString::number(color.alpha()) + ");";
	return style;
}

QString Util::colorToContrastStyleSheet(QColor)
{
	return QString();
}

QString Util::setToString(std::set<int> set)
{
	QString str = "";
	for (auto i : set) {
		str += QString::number(i);
		str += " ";
	}
	return str;
}


std::chrono::high_resolution_clock::time_point tic_time;
void Util::tic()
{
	tic_time = std::chrono::high_resolution_clock::now();
}

double Util::toc()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double, std::milli>(now - tic_time).count();
}

// credits to stackoverflow
double Util::angleWrap(double x)
{
	x = fmod(x, M_PI*2);
	if (x < 0) x += M_PI*2;
	return x;
}

double Util::closestAngle(double x, double y)
{
	return x + angleDiff(y, x);
}

double Util::angleDiff(double x, double y)
{
	double wx = angleWrap(x);
	double wy = angleWrap(y);

	double diff = x - y;

	// small, in range
	if (std::abs(diff) <= M_PI) {
		return diff;
	}

	// shift diff by 2PI so that it's between [-PI, PI]
	if (diff > 0) {
		diff -= 2 * M_PI;
	}
	else {
		diff += 2 * M_PI;
	}
	return diff;
}

double Util::rad(double degree)
{
	const double conv = M_PI / 180.0;
	return degree * conv;
}

double Util::deg(double radians)
{
	const double conv = 180.0 / M_PI;
	return radians * conv;
}

osg::Matrix Util::baseCamera() {
	return osg::Matrix(
		1, 0, 0, 0, // right
		0, 0, 1, 0, // up
		0, -1, 0, 0, // fwd
		0, 0, 0, 1); // trans
}

void Util::quatToYPR(const osg::Quat &quat, double *yaw, double *pitch, double *roll)
{
	osg::Matrix m;
	quat.get(m);

	osg::Vec3 fwd, right, up;
	right[0] = m(0, 0);
	right[1] = m(0, 1);
	right[2] = m(0, 2);
	fwd[0] = -m(2, 0);
	fwd[1] = -m(2, 1);
	fwd[2] = -m(2, 2);
	up[0] = m(1, 0);
	up[1] = m(1, 1);
	up[2] = m(1, 2);
	
	// yaw, rotation of fwd relative to fwd
	double theta = atan2(-fwd.x(), fwd.y());
	// angle from ground looking up
	double phi = asin(fwd.z());

	*yaw = angleWrap(theta);
	*pitch = phi;

	// special case
	// when looking straight up/down theta breaks
	osg::Vec3 fwd_fix;
	bool fixed = false;
	if (doubleEq(phi, M_PI_2)) {
		fwd_fix = -up;
		fixed = true;
	}
	else if (doubleEq(phi, -M_PI_2)) {
		fwd_fix = up;
		fixed = true;
	}
	if (fixed) {
		*roll = 0.0;
		*yaw = angleWrap(atan2(-fwd_fix.x(), fwd_fix.y()));
		return;
	}

	// calculating roll
	// (1)
	// roll is the angle difference between flatright and right
	osg::Vec3 world_up(0, 0, 1);
	osg::Vec3 flat_right = fwd ^ world_up;
	flat_right.normalize();

	double dot = right * flat_right;
	double roll2 = acos(std::clamp(dot, -1.0, 1.0));
	//roll2 = std::clamp(roll2, 0.0, .99999);

	// if right is above the horizontal, then negate
	if (right[2] > 0) {
		roll2 = -roll2;
	}

	// it's backwards... since x is world forward
	*roll = angleWrap(roll2);

	// TODO: roll is untested and unused

	// (2)
	// reverse the yaw rotation
	// reverse the pitch rotation
	// now angle difference from right
}

void Util::matToYPR(const osg::Matrix &mat, double *yaw, double *pitch, double *roll)
{
	Util::quatToYPR(mat.getRotate(), yaw, pitch, roll);
}

osg::Matrix Util::yprToMat(double yaw, double pitch, double roll)
{
	osg::Matrix base = baseCamera();

	// forward vector
	// 1. z: rotate up by sin(pitch)
	// 2. x,y: rotate over from +y

	osg::Vec3 fwd(-sin(yaw)*cos(pitch), cos(yaw)*cos(pitch), sin(pitch));
	fwd.normalize();

	//fwd = osg::Vec3(0, cos(pitch), sin(pitch));

	//qDebug() << "fwd" << fwd[0] << fwd[1] << fwd[2];

	// flat right vector
	osg::Vec3 world_up(0, 0, 1);
	osg::Vec3 flat_right = fwd ^ world_up;

	// rotate flat right based on roll
	osg::Quat roll_quat(roll, fwd);

	// right vector
	osg::Vec3 right = roll_quat * flat_right;
	right.normalize();

	// up vector
	osg::Vec3 up = right ^ fwd;
	up.normalize();

	// the lookat way
	//auto view = osg::Matrix::lookAt(osg::Vec3(0, 0, 0), fwd, osg::Vec3(0, 0, 1));
	//m = osg::Matrix::inverse(view);

	osg::Matrix m(
		right[0], right[1], right[2], 0,
		up[0], up[1], up[2], 0,
		-fwd[0], -fwd[1], -fwd[2], 0,
		0, 0, 0, 1
	);

	return m;

	// somehow you can do this the simple way, but I'm not sure how
	// this doesn't work because ypr is intrinsic, these rotations are extrisic

	//return base
	//	* osg::Matrix::rotate(roll, osg::Vec3(1, 0, 0))
	//	* osg::Matrix::rotate(pitch, osg::Vec3(1, 0, 0))
	//	* osg::Matrix::rotate(yaw, osg::Vec3(0, 0, 1));
}

osg::Quat Util::yprToQuat(double yaw, double pitch, double roll)
{
	return yprToMat(yaw, pitch, roll).getRotate();
}

//osg::Vec4d Util::multVec(osg::Matrixd M, osg::Vec4d v)
//{
//	osg::Vec4d v_new;
//	v_new[0] = osg::Vec4d(M(0, 1), M(0, 2), M(0, 3), M(0, 4)) * v;
//	v_new[1] = osg::Vec4d(M(1, 1), M(1, 2), M(1, 3), M(1, 4)) * v;
//	v_new[2] = osg::Vec4d(M(2, 1), M(2, 2), M(2, 3), M(2, 4)) * v;
//	v_new[3] = osg::Vec4d(M(3, 1), M(3, 2), M(3, 3), M(3, 4)) * v;
//	return v_new;
//}

bool Util::osgMatrixEq(const osg::Matrix m1, const osg::Matrix m2, double epsilon)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			bool different = std::fabs(m1(i, j) - m2(i, j)) > epsilon;
			if (different) return false;
		}
	}
	return true;
}

bool Util::doubleEq(double x, double y, double epsilon)
{
	return fabs(x - y) < epsilon;
}

//4x4 transpose
osg::Matrixd Util::transpose(osg::Matrixd m)
{
	osg::Matrixd result(m);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result(j, i) = m(i, j);
		}
	}
	return result;
}

double Util::simpleCubic(double x0, double x1, double t)
{
	double ht = -2 * pow(t, 3) + 3 * pow(t, 2);
	//qDebug() << "cubic" << ht << lerp(x0, x1, ht);
	return lerp(x0, x1, ht);
}

//Util::endPt Util::hermiteCurve(osg::Vec4d a, osg::Vec4d b, osg::Vec4d da, osg::Vec4d db, double t, double epsl) {
//	osg::Matrixd curveMat(b[0], b[1], b[2], b[3], a[0], a[1], a[2], a[3], db[0], db[1], db[2], db[3], da[0], da[1], da[2], da[3]);
//	osg::Matrixd hermiteMat(-2, 3, 0, 0, 2, -3, 0, 1, 1, -1, 0, 0, 1, -2, 1, 0);
//	double tNext = t + epsl;
//	osg::Vec4d pt1 = multVec((transpose(curveMat) * hermiteMat), osg::Vec4d(t*t*t, t*t, t, 1));
//	osg::Vec4d pt2 = multVec((transpose(curveMat) * hermiteMat), osg::Vec4d(tNext*tNext*tNext*tNext, tNext*tNext, tNext, 1));
//	Util::endPt result;
//	result.pos = osg::Vec3d(pt1[0], pt1[2], pt1[1]);
//	osg::Vec4d diff = pt2 - pt1;
//	result.tan = osg::Vec3d(diff[0], diff[2], diff[1]);
//
//	return result;
//}

bool Util::spheresOverlap(const osg::Vec3f & p1, float r1, const osg::Vec3f & p2, float r2)
{
	// dist < r0 + r1 means intersection
	// square both sides for speed
	float dist2 = (p2 - p1).length2();
	float radius2 = (r1 + r2)*(r1 + r2);
	return dist2 <= radius2;
}


//osg::Matrixd Util::camMatHerm(double t, osg::Matrixd m0, osg::Matrixd m1) {
//	osg::Vec3d pos0 = m0.getTrans();
//	osg::Vec3d pos1 = m1.getTrans();
//
//	Util::endPt res = Util::hermiteCurve(osg::Vec4d(pos0[0], pos0[1], pos0[2], 1), osg::Vec4d(pos1[0], pos1[1], pos1[2], 1), osg::Vec4d(15, 0, 0, 0), osg::Vec4d(15, 0, 0, 0), t, .5);
//	return osg::Matrixd::lookAt(osg::Vec3d(res.pos), osg::Vec3d(res.pos + res.tan), osg::Vec3d(0, 0, 1)); //osg::Matrixd::inverse(
//}

osg::Matrixd Util::cameraMatrixInterp(osg::Matrixd m0, osg::Matrixd m1, double t)
{
	// linear interpolation of position
	osg::Vec3d pos0 = m0.getTrans();
	osg::Vec3d pos1 = m1.getTrans();

	osg::Vec3d pos;
	pos[0] = Util::lerp(pos0[0], pos1[0], t);
	pos[1] = Util::lerp(pos0[1], pos1[1], t);
	pos[2] = Util::lerp(pos0[2], pos1[2], t);

	// linear interpolation of yaw and pitch
	double yaw0, pitch0, roll0;
	double yaw1, pitch1, roll1;
	Util::matToYPR(m0, &yaw0, &pitch0, &roll0);
	Util::matToYPR(m1, &yaw1, &pitch1, &roll1);

	// take the shortest path... if the difference is greater that 180 degrees, then shift to the closer 180
	yaw1 = closestAngle(yaw0, yaw1);
	pitch1 = closestAngle(pitch0, pitch1);
	roll1 = closestAngle(roll0, roll1);

	//qDebug() << "rollin" << yaw0 << pitch0 << roll0;

	double pitch = Util::lerp(pitch0, pitch1, t);
	double yaw = Util::lerp(yaw0, yaw1, t);
	double roll = Util::lerp(roll0, roll1, t);
	osg::Quat rot = Util::yprToQuat(yaw, pitch, roll);

	//std::cout << "yaw " << yaw*180/M_PI << " pitch " << pitch * 180 / M_PI << " roll " << roll * 180 / M_PI << " xyz " << pos << "\n";
	//auto mr0 = osg::Matrix::translate(pos);
	//auto mr1 = osg::Matrix::rotate(rot);
	//auto mr2 = osg::Matrix::rotate(rot) *  osg::Matrix::translate(pos);
	//std::cout << "trans" << mr0 << "rot" << mr1 << "both" << mr2;
	//std::cout << "rot" << mr1;

	//double y0, p0, r0;
	//quatToYPR(rot, &y0, &p0, &r0);
	//std::cout << "** yaw " << y0 * 180 / M_PI << " pitch " << p0 * 180 / M_PI << " roll " << r0 * 180 / M_PI << "\n";


	//std::cout << "up " << mr1(1, 0) << " " << mr1(1, 1) << " " << mr1(1, 2) << "\n";
	//mr1.set(
	//	-0.996293, 0.086027, -0.000173833, 0,
	//	-0.0368997, -0.425514, 0.904199, 0,
	//	0.0777116, 0.900854, 0.427111, 0,
	//	-15.1441, 92.4202, 143.864, 1
	//);
	//mr1.set(
	//	-1, 0,0,0,
	//	0,0, 1, 0,
	//	0,1,0,0,
	//	-15.1441, 92.4202, 143.864, 1
	//);
	//return mr1;
	return osg::Matrix::rotate(rot) * osg::Matrix::translate(pos);
}

double Util::exponentialSmooth(double start, double end, double factor, double dt, double clip)
{
	// amount remaining = .5^2
	double difference = end - start;
	if (std::abs(difference) <= clip) {
		return end;
	}
	return end - difference * pow(factor,dt);
}

double Util::exponentialSmooth2(double * current, double end, double factor, double dt, double clip)
{
	double new_value = Util::exponentialSmooth(*current, end, factor, dt, clip);
	double delta = new_value - *current;
	*current = new_value;
	return delta;
}

QString Util::osgMatrixToQString(osg::Matrix m)
{
	QString s;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			s.append(QString::number(m(i, j)) + " ");
		}
		if (i < 3) s.append("| ");
	}
	return s;
}
