#include "foo.h"
#include <osg/Node>

int Foo::Wow(int x){

	auto node = new osg::Node();
	return 100*x;
}