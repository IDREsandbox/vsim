#include "TypesSerializer.h"

#include <QBuffer>

#include "Core/Util.h"
#include "LockTable.h"

namespace fb = VSim::FlatBuffers;

QColor TypesSerializer::fb2qtColor(const fb::Color &fcolor)
{
	return QColor(fcolor.r(), fcolor.g(), fcolor.b(), fcolor.a());
}

fb::Color TypesSerializer::qt2fbColor(const QColor &qcolor)
{
	return fb::Color(qcolor.red(), qcolor.green(), qcolor.blue(), qcolor.alpha());
}

osg::Vec3f TypesSerializer::fb2osgVec(const fb::Vec3 &fvec)
{
	return osg::Vec3f(fvec.x(), fvec.y(), fvec.z());
}

fb::Vec3 TypesSerializer::osg2fbVec(const osg::Vec3f &ovec)
{
	return fb::Vec3(ovec.x(), ovec.y(), ovec.z());
}

osg::Matrix TypesSerializer::fb2osgCameraMatrix(
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

fb::CameraPosDirUp TypesSerializer::osg2fbCameraMatrix(const osg::Matrix &mat)
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

QPixmap TypesSerializer::readPixmap(const VSim::FlatBuffers::ImageData *d)
{
	if (!d) return QPixmap();
	QPixmap p;

	auto fb_ibuf = d->data();
	auto fb_ifmt = d->format();
	if (fb_ibuf && fb_ifmt) {
		p.loadFromData(fb_ibuf->data(),
			fb_ibuf->size(), fb_ifmt->c_str());
	}

	return p;
}

flatbuffers::Offset<fb::ImageData> 
	TypesSerializer::createImageData(
	flatbuffers::FlatBufferBuilder *builder, QPixmap p)
{
	if (p.isNull()) {
		return flatbuffers::Offset<fb::ImageData>(); // 0 offset
	}
	const char *fmt = "PNG";
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, fmt); // writes pixmap into bytes in PNG format
	const uint8_t *ubuf = reinterpret_cast<const uint8_t *>(bytes.data());
	auto o_data = builder->CreateVector<unsigned char>(ubuf, bytes.length());

	auto o_fmt = builder->CreateString(fmt);

	fb::ImageDataBuilder b_idata(*builder);
	b_idata.add_data(o_data);
	b_idata.add_format(o_fmt);
	auto o_idata = b_idata.Finish();

	return o_idata;
}

QString TypesSerializer::readRelativePath(const flatbuffers::String * string, const Params & p)
{
	if (string == nullptr) return QString();
	QString path = QString::fromStdString(string->str());
	return Util::fixRelativePath(path, p.old_base, p.new_base);
}

flatbuffers::Offset<flatbuffers::String> TypesSerializer::createRelativePath(
	flatbuffers::FlatBufferBuilder * builder,
	const QString & path,
	const Params & p)
{
	QString qpath = Util::fixRelativePath(path, p.old_base, p.new_base);
	return builder->CreateString(qpath.toStdString());
}

