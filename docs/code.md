
# Code stuff

Some things I learned along the way.

## [Model view](./modelview.md)

## Forward declarations

Forward declarations make compilation way faster.
Anything that includes a header has to recompile after the header changes, so headers including headers causes chain-reaction recompilation. To prevent this you can forward declare in headers and only include what you need in cpp files.

If you want to forward declare a unique ptr you have to have a destructor (destructor in header or default destructor needs to access to obj::~(), which is doesn't have yet).

If you want to forward declare something in a namespace you have to write out the namespace {}.

## Disconnecting

Maintaining consistency between gui and data is confusing.

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

The most common ia a one to one connection gui-listens-to-data. ex NarrativeSlideItem::setSlide(). These all over the place and they have a pattern: disconnect current, connect, and null on destruction. Kind of like explicitly written weak pointers. Maybe these should be extracted out and generalized?

Vector collections are templated in Group<T>, which has a bunch of removal and insertion signals.

## Ownership

Qt ownership
- Qt ownership is simple. Parents own a number of children and they are automatically deleted when the parent is. Both need to be QObjects.
- just like unique_ptr, when a QObject's parent is deleted then the object is deleted too
- usually used for static gui stuff: widgets, windows, filters, etc.

unique_ptr
- any QObjects get the destroyed signal
- NarrativeGroup, ERGroup, ModelGroup are singular, don't get thrown in commands. They can be uniquely owned by Root
- doesn't require QObject
- usually clearer sense of ownership (no new, no raw pointers)

shared_ptr ownership
- needed for commands
- infectious: since it's used in commands, also need for Groups

Undo/redo presents a cool use for shared_ptr. By holding onto a shared_ptr on deletion commands we keep objects in memory just as long as they're in the undo stack. Anything where you want fast undo-able creation/deletion should use shared_ptr.

weak_ptr
- EResource->ECategory pointer
- FBOGraphicsWindow->QOpenGLFramebufferObject pointer (deletion requires makeCurrent(), makeCurrent() tests FBO, gets really confusing. FBO has no destroyed signal so making this weak has the same affect)
- NarrativeControl->NarrativeSlide thumbnail pointer (slide can get deleted while painting)

OSG uses its own smart pointers. 
- osg::ref_ptr is a shared pointer (similar to shared_ptr)
- osg::observer_ptr nulls when the object is deleted (similar to weak_ptr)

osg::ref_ptr
- basically shared_ptr, but easier to use (shared_ptr you have to copy other shared_ptrs otherwise you double delete. With osg::ref_ptr you can create them directly from objects, independent from each other, and it still works)
- ref_ptr should be used for osg objects since osg uses them internally.
- you have to inherit osg::Object

raw pointers
- used all over the place
- sometimes it's actually Qt ownership, you can't really tell
- if you're guaranteed the lifetime of pointers it should be ok, ex: controls having app pointer should be safe? maybe it's not good design but it should be safe
- gui listening to data connections

Current:
- commands: shared_ptr
- groups: shared_ptr in Groups and Control
- gui: raw pointer, Qt ownership
- gui->data: raw pointer, disconnect on reassignment
    - given a guarantee that the underlying object won't disappear
    - ^ I don't really like this

## Testability, App State, Refactoring

The code is pretty spaghetti.

I feel like making code testable is much harder than actually testing it.

Ideally the code would be written so that each component is individually testable, no gui, gui alone, both together. Then bots would run regression test on every commit.

I never got around to automated testing. Need to figure out how to build on Linux, need Github permissions, need to set up a Jenkins bot or something similar. I didn't write many tests either.

The only piece which I feel is somewhat testable is the Canvas code. The scene + control are testable with no gui. The canvas can be tested with the scene (Canvas_experiment). The toolbar has no functionality, so could be tested, or could just be data. The editor combines it all together. It's hard to test the editor individually, but there is the CanvasEditor_experiment. The LabelStyle stuff should be testable. 

TODO:

MainWindow, Controls, VSimApp all depend on eachother, which makes them all really hard to test.

I spent a long trying to abstract out an common Control/Editor interface. Currently there is NarrativeControl, NarrativePlayer, ERControl, NavigationControl, BrandingControl.
Each could have a virutal activate() which would give them control of the camera etc, right?

Bunch of unsolved ownership questions:
- the camera is controlled by everyone: navigation, ers, narratives
- canvas visibility is split between: player, narratives
- er visibility is depends on: navigation, ers, are narratives visible?

However... I couldn't figure it out.
I ended up with a solution which works for now, but can get very very convoluted in the future.

Current solution:
Controls depend on app. App has State, which is a big enum deciding who is in control.
Controls listen to state changes and fix up their gui components accordingly.


Ideally an app could exist without the gui. The app can have state, play through a narrative, edit slides, undo-redo etc. all from a command line or tests.

Perhaps VSimApp can be split? I have no clue.

The biggest spaghetti is in the initialization (see main).
App owns controls, controls depend on MainWindow, controls depend on data root, MainWindow depends on app and controls.
1. new MainWindow
2. new App(window): create controls, gives controls window
3. window->setApp(app): 
4. app reset signal -> controls, MainWindow

Ahh I'm so sorry this just happened and I haven't bothered to clean it up.

