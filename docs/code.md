
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

## Disconnecting

Maintaining consistency between gui and data is hard.

Suppose a gui thing is listening to changes in Joe*. The model removes Joe* and emits removed index 3. The gui has to somehow disconnect Joe*, so it would have to maintain a map index->ptr to cleanup. Alternatively you can easily disconnect on aboutToRemove(3).

1. aboutToRemove/removed, aboutToReset/reset
    - about to reset -> null out current reference
    - reset -> load the new thing
2. removed(*)
    - can't really use for resetting, because everything goes
3. keep smart pointer reference
4. create new gui/control objects
    - have to reconnect everything
5. guaranteed lifetime + careful ordering
    - guarantee life of model before calling setGroup()
6. null on destroy event
    - if there are chained listeners to destroy event it breaks

What kind of ownership should we use?

unique_ptr == qt ownership
- same uses, just one doesn't require QObject
- any QObjects get the destroyed signal
- NarrativeGroup, ERGroup, ModelGroup are singular, don't get thrown in commands. They can be uniquely owned by Root

shared_ptr ownership == ref_ptr
- needed for commands
- since it's used in commands, also need for Groups
- Group => we have to use ref_ptr

Current:
- commands: ref_ptr
- groups: ref_ptr in Groups and Control
- gui: raw pointer, disconnect on reassigned
    - given a guarantee that the underlying object won't disappear
    - ^ I don't really like this

