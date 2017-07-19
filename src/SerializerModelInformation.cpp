/*
 * SeriallizerModelInformation.cpp
 *
 *  Created on: October 20, 2012
 *      Author: Franklin Fang
 */

#include <osgDB/ObjectWrapper>

#include "ModelInformation.h"

REGISTER_OBJECT_WRAPPER( ModelInformation,
                         new ::ModelInformation,
                         ::ModelInformation,
                         "osg::Object osg::Node ::ModelInformation" )
{
    ADD_STRING_SERIALIZER(ModelName, "");
	ADD_STRING_SERIALIZER(ReleaseDateVersion, "");
	ADD_STRING_SERIALIZER(ProjectDate, "");
    ADD_STRING_SERIALIZER(PrimaryAuthors, "");
	ADD_STRING_SERIALIZER(Contributors, "");
	ADD_STRING_SERIALIZER(PlaceofPublication, "");
	ADD_STRING_SERIALIZER(AccessInformationURL, "");
	ADD_MATRIXD_SERIALIZER(InitialMatrix, osg::Matrixd());
	ADD_BOOL_SERIALIZER(BrandingOn, true);
	ADD_BOOL_SERIALIZER(LoopOn, true);
	ADD_BOOL_SERIALIZER(CollisionOn, true);
	ADD_BOOL_SERIALIZER(NBarOn, true);
	ADD_BOOL_SERIALIZER(EBarOn, true);
	ADD_BOOL_SERIALIZER(SRCustomized, true);
	ADD_INT_SERIALIZER(SRSelection, 0);
	ADD_INT_SERIALIZER(CustomizedSRX, 1600);
	ADD_INT_SERIALIZER(CustomizedSRY, 1080);
	ADD_BOOL_SERIALIZER(LockExpDate, false);
	ADD_BOOL_SERIALIZER(LockCCreator, false);
	ADD_BOOL_SERIALIZER(LockRestrict2Current, false);
	ADD_BOOL_SERIALIZER(LockRestrictOutput, false);
	ADD_STRING_SERIALIZER(ExpireDate, "");
	ADD_FLOAT_SERIALIZER(FieldView, 55.0f);
	ADD_FLOAT_SERIALIZER(EyeHeight, 1.5f);
	ADD_FLOAT_SERIALIZER(NearClip, 1.0f);
	ADD_FLOAT_SERIALIZER(FarClip, 5000.0f);
	ADD_FLOAT_SERIALIZER(FlightAccFactor, 6.0f);
}