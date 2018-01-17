
# Serialization

## How to

Just copy one of the existing serializers. The osg guide on serialization is pretty helpful.

Make sure you have META_Node and the osg copy constructor in the class. I think this is needed for the ClassName function.


## Compatibility

Be careful with serializers. It's easy to mess up backward/forward compatibility

The ascii format has better compatibility.

Dangerous (everything):

1. Add items to the end of the serializer
2. Change the inheritance list. Making the old narratives an osg::Group broke it and caused us to make Narrative2.
3. Change order of serializers.
4. Remove serializers
5. Multiple inheritance - `MyClass : public QObject, public osg::Node` works, but when you add new items to the serialization list it breaks
