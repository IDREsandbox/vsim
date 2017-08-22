# Changelog

## 2017-08-21
### Added
- Model Outliner (M)
- Time Slider (T)
- First argument to exe is the startup file

### Changed
- MainWindow ui pointer and fwd declarations

## 2017-08-15
### Added
- Import/export for narratives

### Fixed
- QTextEdit + QGraphicsView transparency bug fix

## 2017-08-10
### Added
- Drag and drop for narratives and slides
- Slider motion while dragging along edges
- Move command and signal for Group

## 2017-08-8
### Added
- Undo/redo for narratives and slides
- Undo/redo menu buttons and shortcuts
- Added VSimRoot, does conversion from old .vsim, reduces resposibilities of VSimApp
- Group, NarrativeGroup, ModelGroup replace osg::Group so that signals can come out of them
- Added modelview.md, qteventorder.txt to docs
- Added generic undoable commands for manipulating simple variables, see Command.h

### Changed
- Moved cmake cheatsheet to docs
- Made the slide selection brighter
- Removed execMenu virtual functions in HorizontalScrollBox
- Edit dialog responsibilites given to NarrativeControl

## 2017-07-31
### Added
- shift-B - backface culling
- shift-L - lighting
- shift-X - texturing
- shift-M - polygon mode
- H - resets camera
- G - gravity
- C - collisions (buggy)
- Flight mode strafing width middle mouse or alt
- Hard coded anti aliasing
- 16x9 default window size
- CMake unit testing set up and notes
- CMake installs Qt DLLs
- Added CHANGELOG.md
- Added vertical splitter

### Changed
- Text label dimensions save percentages instead of pixels
- Text label rendering via QTextDocument rather than QLabel
- CMake addition of a virtual library
- Renamed old narratives diagram to old narratives
- MainWindow osg_viewer_widget is now the parent of splitter

### Fixed
- Thumbnail picture is correct

## 2017-07-24
### Added
- First person navigation
- Flight navigation
- Object navigation
