
# Code stuff

Some things I learned along the way.

## [Model view](./modelview.md)

## Ownership, forward declarations

Forward declarations make compilation way faster.
Anything that includes a header has to recompile after the header changes, so headers including headers causes chain-reaction recompilation. To prevent this you can forward declare in headers and only include what you need in cpp files.

Qt ownership is simple. Parents own a number of children and they are automatically deleted when the parent is. Both need to be QObjects.

OSG uses its own smart pointers. 
- osg::ref_ptr is a shared pointer
- osg::observer_ptr nulls when the object is deleted

std::unique_ptr is for simple ownership.
If you want to forward declare a unique ptr you have to have a destructor (destructor in header or default destructor needs to access to obj::~(), which is doesn't have yet).

If you want to forward declare something in a namespace you have to write out the {}.
