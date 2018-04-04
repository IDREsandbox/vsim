
# Serialization

## Flatbuffers

The vsim format was changed from using osg to using flatbuffers. The app tries to read both. Conversion between flatbuffers and the app are explicitly written out (we don't use the buffers directly).

Flatbuffer quirks

- things in tables can be null, so you should to check if strings, structs, and tables are null when reading
- always add to the end of tables
- don't remove from tables
- don't reorder (unless you have explicit ids, look it up)
- there is no version number, but you can always add one
- I made the enum values match our C++ enum values so we can just static_cast<>. This is probably dangerous... anyway if you change the code enums make sure you fix the serializer and write proper conversions.
- there is no inheritance, use unions

flatbuffer definitions
- types.fbs
- eresources.fbs
- narratives.fbs
- settings.fbs
- vsim.fbs

serialization code
- TypesSerializer.cpp
- ERSerializer.cpp
- NarrativeSerializer.cpp
- VSimSerializer.cpp

The things you have to do to add a field (ex. slide::year)
- serialization
    - add line to .fbs
    - add to reader
    - add to writer
    - add to reader/writer tests
- app
    - actually use the thing
    - if undoable - add undo command to change value
    - listen to gui, use command ^
    - if qt modeled - add to some qt model
- gui
    - add to a dialog somewhere

### flatbuffers use options

Flatbuffers is supposed to be zero-copy, so maybe we aren't using it correctly. The current version explicitly copies everything into the app -> lots of extra code.

- use mutable flatbuffers, directly use buffers as data
    - `--gen-mutable --gen-object-api`, see native tables in monster_generated.h example
    - this might mean much less code in the long run
    - I want to try this but it's a ton of work, for now I'm just connecting to existing app
- we can potentially import narratives and ers from a vsim file really quickly since with mmap shenanigans
- should we use vector of unions? or use table wrapping?
    - ex. slide::items consists of images, labels, and potentially other slide item things
    - both are awkward to use
    - C++ is the only language that supports vector of unions, is this something that might go away?

## Serialization options

Things we want
- binary
- fwd/backward compatible, versioning
- easy to change
- serialization shouldn't influence our app code
- code shouldn't influence serialization

Things we don't care about
- speed probably doesn't matter, as long as it isn't models stored in xml

### flatbuffers
- pros
    - binary
    - fwd/backward compatible
    - automatic versioning
    - good support, probably isn't going to change dramatically, break, or be abandoned
    - the formats are self-documenting and independent of the code
- cons
    - lots of extra work
        - redundant enums
        - redundant reader and writer
        - since we have reader/writer, we have to test them
    - it's kind of awkward to work with
        - big namespaces
        - if (notnull)
        - construction of parents has to happen after constructing children
    - file size < 2GB, this leads to a bit of hacking shenanigans
    - extra build step. The cmake and .vcxproj shenanigans are already written
    - extra installation steps, have to install package, have to setup flatc and path
- bonuses, but these probably aren't a big deal
    - cross language - don't have to use C++ :O woah

### osg
The old vsim used built in osg serializers. This had several downsides

- pros
    - representing everything as a Group
        - scroll boxes listen to a generic thing
        - GroupModel means we can make QAbstractItemModels for everything w/o too much work
    - no need for additional libraries
    - just one list, no separate read/write functions
    - moving away from Group means a ton of work
- cons
    - everything has to be an osg::Node of osg::Group
        - leads to tons of dynamic_cast in the code, and null checking (ugly, slow, error prone)
        - code and format is inherently tied to osg
    - annoying code stuff with osg::Node
        - copyop, destructor, META_Node
        - enums have to be inside the class that uses them, restricted to one class
        - have to write getters/setters for everything
        - have to write duplicate getters/setters if member is based on a qt thing or enum
        - writing out the inheritance list as strings, typo -> incompatible forever
        - fixed class names (kind of?), led to Narrative2 rename
        - cmake has to compile serializers in separate step for some reason
        - all of the osg::Node stuff that fills intellisense
        - osg ref ptrs
    - hard to test? never really tried
    - no version number, no forward/backward binary compatibility
        - every serializer change -> all files are now incompatible
- Advantages of not using osg
    - we can swap out the model format
    - we can swap out the renderer
    - we can swap out all of the code
    - it's not like we're going to, but it's nice to know that the format can live beyond the code

### boost::serialization

I had trouble picking between this and flatbuffers. This is definitely easier to use, and would mean less code.

- pros
    - easy to use
    - just one serializer definition, goes both ways (no need for separate read/write functions)
    - version per type
    - explicit version if/else stuff is easy to understand
- cons
    - no cross platform binary format? there is but it isn't supported?
    - not very active
    - installing boost is kind of crazy, using vcpkg isn't so bad though

### cereal

This is a lot like boost serialization. It's supposed to be faster. It's not very active.

### Qt

You can write qt stuff to/from streams. It could be useful to write QFont and stuff instead of having all the manual work that we have for LabelStyles.

### Google Protocol Buffers

This is meant for lots of small messages over networks, our thing is kind of big
