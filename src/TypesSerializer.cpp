#include "TypesSerializer.h"
#include "Util.h"

namespace fb = VSim::FlatBuffers;

QColor Serializer::fb2qtColor(const fb::Color &fcolor)
{
	return QColor(fcolor.r(), fcolor.g(), fcolor.b(), fcolor.a());
}

fb::Color Serializer::qt2fbColor(const QColor &qcolor)
{
	return fb::Color(qcolor.red(), qcolor.green(), qcolor.blue(), qcolor.alpha());
}

osg::Vec3f Serializer::fb2osgVec(const fb::Vec3 &fvec)
{
	return osg::Vec3f(fvec.x(), fvec.y(), fvec.z());
}

fb::Vec3 Serializer::osg2fbVec(const osg::Vec3f &ovec)
{
	return fb::Vec3(ovec.x(), ovec.y(), ovec.z());
}

osg::Matrix Serializer::fb2osgCameraMatrix(
	const fb::CameraPosDirUp &buffer)
{
	osg::Vec3f dir = fb2osgVec(buffer.dir());
	osg::Vec3f pos = fb2osgVec(buffer.pos());
	osg::Vec3f target = dir + pos;
	osg::Vec3f up = fb2osgVec(buffer.up());

	osg::Matrix view = osg::Matrix::lookAt(pos, target, up);
	osg::Matrix mat = osg::Matrix::inverse(view); //strange, how do you use inverse
	return mat;
}

fb::CameraPosDirUp Serializer::osg2fbCameraMatrix(const osg::Matrix &mat)
{
	osg::Vec3f eye;
	osg::Vec3f target;
	osg::Vec3f up;
	osg::Matrix view = osg::Matrix::inverse(mat);
	view.getLookAt(eye, target, up);

	fb::Vec3 s_pos = osg2fbVec(eye);
	fb::Vec3 s_dir = osg2fbVec(target - eye);
	fb::Vec3 s_up = osg2fbVec(up);

	return fb::CameraPosDirUp(s_pos, s_dir, s_up);
}