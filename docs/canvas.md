# Canvas

The canvas holds text boxes and images.

CanvasScene : QGraphicsScene
- has CanvasItems
- build in operations:
    - transform (sRectsTransformed)
    - text editing (sDocumentCommand)

CanvasContainer
- has QGraphicsView
- has TransformManipulator

CanvasControl
- performs operations on a CanvasScene
- pushes commands to undo stack
- command wrapped operations:
    - add label by type
    - add image
    - remove canvas item
    - wrap transform
    - wrap text edit
    - set border color
    - set border width
    - etc...

- this was made so that it could be tested with the scene, independent from gui controls (toolbar), independent from viewer (container)

CanvasToolBar
- a bunch of public buttons

CanvasEditor
- connects everything together in a nice box, to be used to NarrativeControl or MainWindow
- connects CanvasToolBar w/ CanvasControl
- has a CanvasContainer for viewing


## Decision, changes, how things work

### Should NarrativeSlideItem be separate from CanvasItem?

Why together:
- Less redundancy, don't need mapping

Why separate:
- Avoid inheriting lots of Qt garbage (remember how inheriting osg was a pain)

This is how it used to be. NarrativeCanvas maintained a map<NarrativeSlideItem,CanvasItem>.

Data (slide, slideItem) <> NarrativeControl <> NarrativeCanvas <> CanvasContainer

There is just a ton of redundancy, since a SlideItem and and CanvasItem are essentially the same thing.

### Selection and Focus

There are two kinds of selection in a canvas.
1. item selection - one or more CanvasItems are selected
2. text subselection - one CanvasLabel is selected, and the internal text is focused
3. (no selection)

ToolBar ComboBoxes are set to Qt::NoFocus, so when you're editing a label and changing its font it doesn't lose focus. This breaks arrow key font selection.

CanvasControl::subText() checks or subselection
CanvasControl::multiText() checks for labels selected

### Live ToolBar

The toolbar should change based on what is selected.

Current:
  border width
  font
  font size

Optional, TODO:
  bold, italic, underline, strikeout, alignment, colors, etc

Signaling

Fine grained?
- sAllBorderWidthChanged()
- sAllFontChanged()
- sAllFontSizeChanged()
- sAllStyleChanged()

Loose grained?
- sSelectionChanged()

It doesn't really matter. I'm doing fine grained

### Font previewing, Font size previewing, Border previewing

It would be nice... but it's a ton of work. Google slides doesn't have it.

You need mergable commands. See QUndoCommand::mergeWith()
  This means means selection wrapping with ICommandStack::push() doesn't work because the types have to be clean. Perhaps have a hierarchal mergeWith()

When is a command mergable?
- edit widget has focus
- run a timer (~2 sec)
- command is at the top
- same command type, same selection involved

Have to add a MergeTracker() to track stack changes and run a timer

Add a ComboBoxWrapper() to add sPreview and sSubmit signals. Manage a timer.

Gets really complicated for text wrappers
When you change the font for multiple text boxes you call beginEditBlock() on each, then when you undo you have to avoid accepting the undoCommandAdded() signal, call endEditBlock(), and undo each.