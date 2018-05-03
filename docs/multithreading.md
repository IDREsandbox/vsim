
# Qt multithreading

QObjects and threading

- Signals and slots work across threads in queues. Types passed must be registered with QVariant
- QObjects have a current thread. When you operate on them it should be from the current thread.
- Use moveToThread() from current thread to push into a different thread (can't pull).
- A QObject hierarchy must all be in a single thread.
- Using moveToThread() propagates from parents to children, but not members or null parented objects
- QWidgets should be on the main thread

QThread + QObject

- you can't delete QThread before finished -> something has to wait on QThread to finish before -> have Worker/Pool/Controller destructor wait on thread with thread.quit()
- used for saving

QtConcurrent

- convenient, futures
- you can allocate, moveToThread to main, and return (can't use unique_ptr so it feels leaky)
- used for loading: allocate new VSimRoot in thread, load the stuff, move new root to main thread
- can't call moveToThread() into the future because there it's not exposed -> can't move VSimRoot into a QtConcurrent thread -> can't use for saving

Uses

- FutureDialog: execs a modal dialog, waits on a Future or QThread to finish
- SimpleWorker: owns a thread, runs a lambda
- Loading: allocate new VSimRoot in thread, load the stuff, move new root to main thread
- Saving: move VSimRoot into saver thread, save, move back

Safety

- Our current uses are pretty unsafe. We just rely on a QDialog exec() to prevent any user actions that might race with the saving/loading thread. You could easily break it with state manipulation on a QTimer or something (narrative player).

# Our MVC + moveToThread

- Our data is a hierarchy of QObjects emitting change signals - Narratives, EResources, Slides, etc.
- We use shared_ptr to managed ownership (because of undo/redo), call them shared objects
Our hierarchal signal model + moveToThread

Apparently we're not supposed manage QObject like this. Shared objects have null parents, so they're not a part of a QObject hierarchy, so a simple call to VSimRoot::moveToThread doesn't work.

## Solutions:

- Rewrite everything without QObject
    - have a signalling layer between data and gui NarrativeSignals : GroupSignals, SlideSignals, etc
    - too much work just to support threaded loading
    - if you add some big components in the future maybe try this out
- Add shared objects to VSimRoot hierarchy
    - Main issue: shared_ptr and ~QObject deletes everything twice -> crashes
    - Solution: Don't delete children on ~QObject(). See WeakObject.h
    - Whenever you create a shared object call, std::make_shared<>()
    - Safest solution
- Manually move all shared objects to thread
    - see VSimRoot::moveAllToThread(QThread*)
    - least amount of work

Adding shared objects to the hierarchy is a lot of work...
- give everything a (QObject *parent) constructor (I ended up doing this)
- replace all make_shared<>() with make_shared<>(bucket)
- add WeakObject buckets all over the place OR spread a global bucket to all controls somehow
I started with this then got lazy halfway through. See VSimRoot::moveAllToThread()

# OSG

OSG multithreading doesn't work with GraphicsWindowEmbedded.

# Potential races

I haven't done any mutexing or anything yet... So these are just hypothetical

- QTimers affecting state and save/load threads
- osgViewer::frame() and save/load threads, if frame() affects the scene graph state is could be a problem
