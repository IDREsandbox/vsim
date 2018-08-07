#ifndef TYPESSERIALIZER_H
#define TYPESSERIALIZER_H

#include <flatbuffers/flatbuffers.h>
#include "types_generated.h"
#include <QColor>
#include <osg/Matrix>

class EResourceGroup;

namespace VSim {
namespace FlatBuffers {
	struct Vec2;
	struct Vec3;
	struct CameraPosDirUp;
	struct Color;
}
}

namespace TypesSerializer {
	QColor fb2qtColor(const VSim::FlatBuffers::Color &fcolor);
	VSim::FlatBuffers::Color qt2fbColor(const QColor &qcolor);

	osg::Vec3f fb2osgVec(const VSim::FlatBuffers::Vec3 &fvec);
	VSim::FlatBuffers::Vec3 osg2fbVec(const osg::Vec3f &ovec);

	osg::Matrix fb2osgCameraMatrix(const VSim::FlatBuffers::CameraPosDirUp &buffer);
	VSim::FlatBuffers::CameraPosDirUp
		osg2fbCameraMatrix(const osg::Matrix &mat);

	struct Params {
		// for changing relative paths
		// need both for model write
		// need new for model read
		QString old_base;
		QString new_base;
	};
	QString readRelativePath(const flatbuffers::String *string, const Params &p);
	flatbuffers::Offset<flatbuffers::String> createRelativePath(
		flatbuffers::FlatBufferBuilder *builder,
		const QString &path,
		const Params &p);
}

//void readEResource();
#endif