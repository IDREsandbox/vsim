# Changelog

## 2018-4-6
- .vsim uses Google Flatbuffers for serialization
- import/export narratives, old narratives, undoable
- import/export resources, old resources, undoable
- model information, dialog, serialization

## 2018-3-20
### Added

#### Narratives
- narrative styles added
- 'p' to play
- right arrow or 'p' to advance slide
- left arrow to go back a slide
- 'esc' to stop
- slides fade in
- thumbnails sped up a bit, fixed with new canvas
- selections stored as a stack so deselection gives predictable behavior
- can play narratives without opening the slides

#### Slide/Canvas Overhaul
- easily move and resize slide items
- multi-editing with undo/redo
- fixed disappearing labels issue with old canvas

#### App State Reform
- app state cleaned up a lot, allowing for more clear behavior
- fade out canvas when navigating and messing with ERs
- selecting canvas deselects irrelevant slides
- selecting narrative things deselects ERs
- ER and canvas can't appear at the same time
- navigation stops on er/nar/edit events

#### Embedded Resources
- sped up a bit
- fixed some issues with category deletion

#### Other
- added use of QActions
- use Qt 5.10.0, 5.9 doesn't have off-screen rendering, 5.10.1 has a QGraphicsOpacityEffect bugs
- saving an EResource saves an index the ECategory rather than the objects, the two are reconnected after loading

## 2018-1-16
### Added
#### Embedded Resources
- filtering
- sorting
- undo/redo
- goto and set
- open
- open relative paths
- TODO: import/export
- ECategory editing and undo/redo

### Changed
- Changing navigation mode unfreezes
- Time slider default on
- Model outliner starts expanded
- Strafing doesn't take mouse cursor
- Flight mode speed scaling

## 2017-11-21
### Added
- Embedded resource positioning

### Fixed
- Loads old embedded resources

## 2017-08-25
### Added
- Basic embedded resources
- Canvas undo/redo

### Fixed
- Disappearing player bug?

## 2017-08-23
### Changed
- Narrative slide transitions use a cubic polynomial
- NewNodeCommand has getNode for initialization stuff

### Fixed
- Narrative player code cleaned up a bit, see the state machine diagram
- Selections cleaned up a bit

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
- Shift selection for scroll boxes

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
