#include <algorithm>
#include <QDebug>
#include "Util.h"

#include <osg/Matrix>
#include <osg/io_utils>
#include <iostream>
#include <chrono>

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

QString Util::setToString(std::set<int> set)
{
	QString str = "";
	for (auto i : set) {
		str += QString::number(i);
		str += " ";
	}
	return str;
}

std::vector<int> Util::fixIndices(const std::vector<int> &fixme, const std::set<int>& insertions)
{
	size_t max_index = 0;
	for (int x : fixme) {
		max_index = std::max((size_t)x, max_index);
	}
	// build delta_array, [+0, +0, +1, +1, +2, +2, +2]
	std::vector<size_t> delta_array(max_index + 1, 0);
	size_t shift = 0;
	auto it = insertions.begin();
	size_t old_index = 0;
	size_t new_index = 0;
	while (old_index < delta_array.size()) {
		if (it == insertions.end() || new_index != *it) {
			delta_array[old_index] = shift;
			old_index++;
			new_index++;
		}
		else {
			shift++;
			it++;
			new_index++;
		}
	}
	//for (size_t i = 0; i < delta_array.size(); i++) {
	//	if (it != insertions.end() && i == *it) {
	//		shift++;
	//		it++;
	//	}
	//	delta_array[i] = shift;
	//}
	std::vector<int> result(fixme);
	for (size_t i = 0; i < result.size(); i++) {
		size_t old_index = result[i];
		result[i] = old_index + delta_array[old_index];
	}
	return result;
	// Set version, I was wondering which would be faster
	// Should be O(nlogn) vs O(n), apparently this is really slow even for small n
	// ex: insert [1, 4, 6]
	// -> (1: 0, 4: 1, 6: 2)
	// < 1 gets bumped +0
	// >= 1, < 4 gets bumped +1
	// >= 4, < 6 gets bumped +2
	//std::map<int, int> index_to_delta;
	//int delta = 0;
	//for (int index : insertions) {
	//	index_to_delta[index] = delta;
	//	delta++;
	//}
	//std::vector<int> result(fixme);
	//for (size_t i = 0; i < result.size(); i++) {
	//	int x = result[i];
	//	auto it = std::upper_bound(insertions.begin(), insertions.end(), x);
	//	int d;
	//	if (it == insertions.end()) {
	//		d = insertions.size();
	//	}
	//	else {
	//		d = index_to_delta[*it];
	//	}
	//	result[i] = x + d;
	//}
	//return result;
}

std::vector<int> Util::fixIndicesRemove(const std::vector<int>& fixme, const std::set<int>& changes)
{
	if (fixme.size() == 0) return {};
	int max = *std::max_element(fixme.begin(), fixme.end());
	std::vector<int> delta_array(max + 1, 0);
	int shift = 0;
	for (size_t i = 0; i < delta_array.size(); i++) {
		if (changes.find(i) != changes.end()) {
			shift--;
		}
		delta_array[i] = shift;
	}

	std::vector<int> result(fixme);
	for (size_t i = 0; i < result.size(); i++) {
		size_t old_index = result[i];
		result[i] = old_index + delta_array[old_index];
	}

	return result;
}

std::vector<std::pair<size_t, size_t>> Util::clumpify(const std::vector<size_t>& indices)
{
	if (indices.size() == 0) return {};
	size_t start = indices[0];
	size_t prev = indices[0];
	std::vector<std::pair<size_t, size_t>> output;

	for (size_t x : indices) {
		// continue case
		if (x == prev + 1) {
		}
		// make range
		else if (x > prev + 1) {
			output.push_back({start, prev});
			start = x;
		}
		// first item, or a bug if unsorted
		else {
		}
		prev = x;
	}
	// always make range at end
	output.push_back({ start, prev });
	return output;
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
	if (x < 0)
		x += M_PI*2;
	return x;
}

double Util::closestAngle(double x, double y)
{
	// already the closest?
	if (std::abs(x - y) <= M_PI) {
		return y;
	}
	else {
		if (x > y) {
			// going down is too far, go up instead
			return y + 2 * M_PI;
		}
		else {
			// going up is too far, go down instead
			return y - 2 * M_PI;
		}
	}
}

void Util::quatToYPR(const osg::Quat &quat, double *yaw, double *pitch, double *roll)
{
	osg::Matrix m;
	quat.get(m);

	osg::Vec3 fwd, right;
	right[0] = m(0, 0);
	right[1] = m(0, 1);
	right[2] = m(0, 2);
	fwd[0] = -m(2, 0);
	fwd[1] = -m(2, 1);
	fwd[2] = -m(2, 2);
	
	// yaw, rotation of fwd relative to fwd
	double theta = atan2(-fwd.x(), fwd.y());
	// angle from ground looking up
	double phi = asin(fwd.z());

	*yaw = angleWrap(theta);
	*pitch = phi;

	//std::cout << "fwd " << fwd << "\n" << "fwdz " << fwd.z() << " pitch " << *pitch * 180/M_PI << " yaw " << fwd.z() << "\n";

	// calculating roll
	// roll is the angle difference between flatright and right
	osg::Vec3 vertical(0, 0, 1);
	osg::Vec3 flatright = fwd ^ vertical;
	flatright.normalize();
	
	double roll2 = acos(right * flatright);
	// if right is above the horizontal, then negate
	if (right[2] > 0) {
		roll2 = -roll2;
	}

	// it's backwards... since x is world forward
	*roll = angleWrap(roll2);
	*roll = 0; // TODO FIXME HACK, fix later
}

osg::Quat Util::YPRToQuat(double yaw, double pitch, double roll)
{
	// start with the camera facing forward in the world
	osg::Matrix base(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1);
	// * osg::Matrix::rotate(yaw, osg::Vec3(0, 0, 1))
	return (base * osg::Matrix::rotate(pitch, osg::Vec3(1, 0, 0)) * osg::Matrix::rotate(yaw, osg::Vec3(0, 0, 1))).getRotate();
	//return (base * osg::Matrix::rotate(osg::Quat(roll, osg::Vec3(1, 0, 0), pitch, osg::Vec3(0, 1, 0), yaw, osg::Vec3(0, 0, 1)))).getRotate();
}

osg::Vec4d Util::mult_vec(osg::Matrixd M, osg::Vec4d v)
{
	osg::Vec4d v_new;
	v_new[0] = osg::Vec4d(M(0, 1), M(0, 2), M(0, 3), M(0, 4)) * v;
	v_new[1] = osg::Vec4d(M(1, 1), M(1, 2), M(1, 3), M(1, 4)) * v;
	v_new[2] = osg::Vec4d(M(2, 1), M(2, 2), M(2, 3), M(2, 4)) * v;
	v_new[3] = osg::Vec4d(M(3, 1), M(3, 2), M(3, 3), M(3, 4)) * v;
	return v_new;
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

Util::endPt Util::hermiteCurve(osg::Vec4d a, osg::Vec4d b, osg::Vec4d da, osg::Vec4d db, double t, double epsl) {
	osg::Matrixd curveMat(b[0], b[1], b[2], b[3], a[0], a[1], a[2], a[3], db[0], db[1], db[2], db[3], da[0], da[1], da[2], da[3]);
	osg::Matrixd hermiteMat(-2, 3, 0, 0, 2, -3, 0, 1, 1, -1, 0, 0, 1, -2, 1, 0);
	double tNext = t + epsl;
	osg::Vec4d pt1 = mult_vec((transpose(curveMat) * hermiteMat), osg::Vec4d(t*t*t, t*t, t, 1));
	osg::Vec4d pt2 = mult_vec((transpose(curveMat) * hermiteMat), osg::Vec4d(tNext*tNext*tNext*tNext, tNext*tNext, tNext, 1));
	Util::endPt result;
	result.pos = osg::Vec3d(pt1[0], pt1[2], pt1[1]);
	osg::Vec4d diff = pt2 - pt1;
	result.tan = osg::Vec3d(diff[0], diff[2], diff[1]);

	return result;
}

osg::Matrixd Util::camMatHerm(double t, osg::Matrixd m0, osg::Matrixd m1) {
	osg::Vec3d pos0 = m0.getTrans();
	osg::Vec3d pos1 = m1.getTrans();

	Util::endPt res = Util::hermiteCurve(osg::Vec4d(pos0[0], pos0[1], pos0[2], 1), osg::Vec4d(pos1[0], pos1[1], pos1[2], 1), osg::Vec4d(15, 0, 0, 0), osg::Vec4d(15, 0, 0, 0), t, .5);
	return osg::Matrixd::lookAt(osg::Vec3d(res.pos), osg::Vec3d(res.pos + res.tan), osg::Vec3d(0, 0, 1)); //osg::Matrixd::inverse(
}

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
	Util::quatToYPR(m0.getRotate(), &yaw0, &pitch0, &roll0);
	Util::quatToYPR(m1.getRotate(), &yaw1, &pitch1, &roll1);

	// take the shortest path... if the difference is greater that 180 degrees, then shift to the closer 180
	yaw1 = closestAngle(yaw0, yaw1);
	pitch1 = closestAngle(pitch0, pitch1);
	roll1 = closestAngle(roll0, roll1);

	//qDebug() << "rollin" << yaw0 << pitch0 << roll0;

	double pitch = Util::lerp(pitch0, pitch1, t);
	double yaw = Util::lerp(yaw0, yaw1, t);
	double roll = Util::lerp(roll0, roll1, t);
	osg::Quat rot = Util::YPRToQuat(yaw, pitch, roll);
	

	//std::cout << "yaw " << yaw*180/M_PI << " pitch " << pitch * 180 / M_PI << " roll " << roll * 180 / M_PI << " xyz " << pos << "\n";
	//auto mr0 = osg::Matrix::translate(pos);
	auto mr1 = osg::Matrix::rotate(rot);
	//auto mr2 = osg::Matrix::rotate(rot) *  osg::Matrix::translate(pos);
	//std::cout << "trans" << mr0 << "rot" << mr1 << "both" << mr2;
	//std::cout << "rot" << mr1;

	double y0, p0, r0;
	quatToYPR(rot, &y0, &p0, &r0);
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
		s.append("| ");
	}
	return s;
}
