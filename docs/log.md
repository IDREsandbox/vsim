Like a changelog but with more details.

# 4/13/2018

I'm fixing filter sort proxy and I don't know if I should use qt chunk signals or my own.

Speed texting abstract item model, to check chunk speed

Debug - 

detatched, hidden, 4k, remove random - 45ms

Release

detached, hidden, 5k, random - 9ms
attached, hidden, 5k, random - 80-130ms (too slow?)
detached, visible, 5k 

aborttttt 
do this later
premature optimization right?

# 4/12/2018

old vsim
`find . -name '*.cpp' -o -name '*.h' | xargs wc -l`
25055

new vsim
`find . -name '*.cpp' -not -path './windows/*' -o -name '*.h' -not -path './windows/*' -o -name '*.inl' | xargs wc -l`
20668

cool, we're smaller, thats kind of a good thing
some things are missing
- .ui files
- .fbs files
- moc files, uic files
- cmake

We have
- new
    - more docs
    - binary compatibility
    - some tests
    - cmake, vcpkg instructions (potential CI)
    - undo/redo
    - rich text
    - multi-edit
    - drag and drop
    - consistent state, doesn't crash?
    - right click menus
    - speed scaling, strafing
    - model outliner
- old
    - faster rendering (in progress)
    - anti aliasing (in progress)
    - er positioning (in progress)
    - locking

How much work has it been?
I started part time work 2017 February.
Almost-full-time jul-aug, oct-april. I work like 60 hours a week?
 8 months? there was a ton of slacking oct/nov/dec/feb

Things I've learned
- testing is great
- dumping osg was great
- model view controller
- qt, osg, cmake, flatbuffers
- big state enum saves a lot of headache
- templates -> no dynamic cast, but also slower compile, and are kind of infectious
