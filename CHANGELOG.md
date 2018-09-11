# Changelog

## 2018-9-10

- Added x/y/w/h to style editor
- Added "Paste as plain text" to canvas text editing

## 2018-8-30

- Menu tabs moved around. Removed Window menu, moved History button to Edit menu, moved others into Model.
- Near clip issues fixed
- Navigations settings saves last selected preset
- Added Navigation > Set Position. Opens a dialog to manually type in position.
- Updated about VSim text
- Window size initialization is less crazy
- Fixed dialog titles and question marks (Model Information, Switches, Fonts & Colors, About, ER Edit, Position, ER Filters, Time Slider, Export, Navi Settings, Disp Settings)
- Removed other settings
- Replaced icons, added .exe icon
- Fonts & colors greys out when a narrative is not active
- Fixed branding editor transform desyncing after resize
- ER search by file name, ER search by author
- ER scroll bar style
- ER double click -> don't go to
- Right click always clears ERs (experimental)
- ER filter rearrangement
- ER filter options are saved with the file
- Added x/y/w/h to canvas editor
- Fixed importing linked files with ProxyNodes (I think?)

## 2018-8-27
- When importing/opening a model with referenced models, NodeProxys are converted to plain Groups (forcefully embedding)

## 2018-8-24

- Thumbnails save w/ file, repaint when done editing
- Thumbnails draw with current display settings
- Expiration date on files
- Lighting off by default
- Lighting serialize
- Current year saves with file
- ER filter window
- ER sort serialize
- ER all sort
- ER slow down (.6 sec)
- ER fade in/out
- ER display titles adjustment
- ER reposition icon on left

### Fixes

- Style settings border width fix
- Canvas right click dotted fix

### Code stuff
- Deleted mrichtextedit

## 2018-8-23

### Locking

- Lock file
    - lock settings
    - model information
    - models, import/edit/remove
- Lock settings
    - camera settings
    - window settings
    - navigation settings
- Restrict to current
    - add/remove nars
    - add/remove ers
    - add/remove/edit er categories
    - import nars
    - import ers
- Disable Navigation
- Narrative Lock
    - add/remove slides
    - edit slides
    - slide duration, transition, drag and drop, etc
    - right click menu, password
    - lock icon
    - export all, with password
- ER Lock
    - edit er dialog
    - set position
    - right click menu, password
    - lock icon
    - export all, with password
- File > Export vsim file
- Settings > Lock Settings
- Lock all nars/ers
- Embed all models

### Fixes

- Fixed loading models w/ relative paths + crash
- Fixed ER urls changing on save/load

### Misc

- Added new icons for reposition and text ERs
- Renamed ER "Open" to "Launch"

## 2018-8-7

- Can remove models
- Added option to reference an external model vs embed the model in vsim file
- ER relative paths and model relative paths fix automatically when saving vsim file in a different folder
- Extended model information
- Fixed auto reposition when loading old resources

## 2018-7-27

### Added

- Switches

### Touchups

- ER display bigger
- ER display shows category, removed author
- Right click anywhere clears all ERs
- Branding resizes with top/bottom bars (experiment)
- Ground mode enables height collision test, added ground mode
- Canvas editing text button highlighting (bold, italic, etc)
- Rearranged canvas toolbar
- Canvas item minimum border width 1, 0 means no border
- Narrative author shows elipses if too long
- Added narrative player pausing (p)
- Left click works with narrative player "click to continue"

## 2018-7-20

- Reworked thumbnails
- OSG rendering docs
- Style settings has image tab
- Added navigation settings, save with file
- Added camera settings, save with file
- Window size saves with file
- Narrative and ER bars save with file

### Fixes

- Fixed crash involving slides not updating after opening a file
- Loads old ER auto launch properly

## 2018-7-6

- Basic anti aliasing, anti aliasing settings
- Moved menus around, added Model dropdown
- Added checkbox to disable auto launching ers
- Can select all in an ER box with ctrl-a
- Added branding, see Model > Edit Branding
- Made gravity smoother
- Added height based collision, walking through scenes is more natural
- Pressing space after editing/playing unfreezes rather than refreezes
- Changed speed tick limit to +/- 40 (x1000 to 1/1000)

### Fixes
- Narrative player resumes on the correct slide after flying around
- Fixed slide drag and drop getting stuck
- Fixed some random crashes

## 2018-6-29

### Embedded resources
- Added "All Resources" scroll box
- Added index number to ERs
- Added auto launching, this might cause old files to lose their auto launch settings
- Disabled auto launching for global resources
- ER display: right click closes display
- ER display: scroll bar added to description area
- Edit ER dialog: initial global/local depends on which box you clicked on
- Edit ER dialog: 40 character limit to ER edit dialog
- Edit ER dialog: added set position button
- ER scroll box: fx icon hidden
- ER scroll box: distance hidden
- Fixed launching ER files with spaces

### Narratives
- Hyperlinks
- Borders around slide editor buttons
- Arrow keys work when editing labels
- Fixed 'None' style related crash
- Added border color and border width to style settings dialog
- Fixed preview in style settings dialog
- Narrative titles shrink to fit, narrative description ellipses

### Misc
- Added hpr to the status bar

## 2018-5-24

### Added
- Canvas editor toolbar
- Canvas images
- Image and label borders

## 2018-5-4

### Added
- Status bar shows navigation mode changes, freeze/unfreeze
- Status bar shows current coordinate
- Create label buttons are styled
- Loading/saving doesn't freeze the screen
- Current coordinate shows in status

### Fixed
- Launching ERs with absolute file paths
- Fixed model outliner

## 2018-4-26

### Embedded resources
- ER box sped up dramatically (see karnak)
- Local resources appear/disappear
- Autolaunch
- Goto on click (autoreposition)
- Time slider affects local resources
- Filtering - sort by years, sort by distance
- Filtering - use years
- Filtering - show all local
- ER box - shows distance
- ER box - selection highlighting
- ER box - icons for type, popup, and reposition
- ER display - goto, open, close, close all
- ER set position multiedit

### Other
- About dialog
- OSG render settings (see Render menu)
- Hide debug menu (F11 to show)
- Remove osg::Group dependency and dynamic_casts
- Use vcpkg for dependencies

### Fixes
- First person mode releases mouse when opening time slider
- First narrative plays if nothing is selected

### Broken
- Thumbnails broken
- Model outliner broken

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
