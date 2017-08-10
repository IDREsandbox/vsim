
##Qt Model View

has its own ModelView framework
It's organized in tables, with columns and rows
Hierarchy is possible with parents

- It's possible to make Item classes for each data class, ex. NarrativeItem which maps the indexes to data... but maybe it's too much work?

Used for:

- Models?


##Our Model View

Narratives have a custom MVP-like structure


What is our own?
I played with a bunch of different designs... they all have a lot of repetition


###Model

The model is the osg tree. Data nodes like NarrativeSlide emit signals when data changes. Commands operate on the data and allow for undo/redo of changes. There are generic template commands in Commands.h.

Group operations such as adding/removing items are handled by the Group class. There are generic undoable commands for adding/removing nodes from a group (see Group::NewNodeCommand<T>).


###Control

The control takes all edit signals from the scroll boxes, buttons, window actions, etc. and maps them to commands and command macros.

The control also keeps track of the current narrative and current slide. It relinks the scroll boxes and canvas when the active narrative/slide change.

TODO: The control keeps track of selection


###Views

Views are the gui parts. They listen to data changes and change the gui accordingly.

HorizontalScrollBox is a view of Group. The connection is made with setGroup. sNew() and sDelete() signals cause the box to create new items with the virtual method createItem. The narrative box creates NarrativeScrollItems and the slide box creates SlideScrollItems.

NarrativeScrollItem is a view of a single narrative. The connection is made with setNarrative(Narrative2*). The item listens to data changed signals like sAuthorChanged(string) and reacts by changing the gui.

Edit signals often don't directly manipulate the gui. For example, Edit Duration on a SlideScrollItem -> Edit Duration (index) on SlideScrollBox -> Edit Duration creates a dialog, returns from dialog creates SetDurationCommand (Control) -> setDuration (NarrativeSlide) -> sDurationChanged signal (NarrativeSlide) -> duration label setText (SlideScrollItem)


##Considerations

###Why model/view/something?

- Technically the goal is to make it easier to test... but we don't have unit tests so that doesn't really matter.
- I mostly wanted a flow that makes sense, isn't too repetitive, has some separation.

###Why do we have our own?

- Multiple types (Narratives/Slides/Labels) in a fixed hierarchy. For a Qt Model View this means dynamic casting all over the place.
- The indexing for the Qt model meant big switch tables. (3 types) x (k data items) x (n data operations) = big code mess of switch cases.
- Not necessary. We don't really need the default views for any of the narrative data. A tree view might be nice but it's unnecessary.
- Undo/redo is a pain with Qt models

###Why does the control exist? Edit signals could pass directly from items to data

- Originally the reason for the control was because the little +/- buttons don't know anything about Narratives or selections. There had to be some class that links buttons to the scroll boxes and to the data.
- Set operations are easier from a bigger control class. Macros make it so that the model can remain free of set operations, hence simpler.
- Selection undo/redo is (probably) easier from a bigger control class.
- The need for an owner of current narrative, current slide, selection level (narratives or slides or labels), selection sets.


TODO: explain this

- Narratives/Slides/Labels
- Embedded Resources