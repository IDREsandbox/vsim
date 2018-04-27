
# osg stuff

navigation modes

There are 3 navigation modes
Frozen and unfrozen
Widget has/doesn't have focus

osg viewer has camera manipulators
There are 4 manipulators

The viewer maps the navigation modes to camera manipulators.

## state sets

Render settings are in StateSet, controlled by osgGA::StateSetManipulator.

## multiple views

If you ->frame() two different views of the same root group it crashes...
Solution?
You can use CompositeViewer and swap out views on ->frame but it's really slow.

## multithreading

