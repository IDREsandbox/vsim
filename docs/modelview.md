
# Model View

See the draw.io link for some pictures

## Qt Model View

Qt has its own ModelView framework.
It's organized as hierarchal tables, with columns, rows, and parents
If you wrap data into a Qt Model you get a bunch of GUI elements for free (tree/table/list views)
View items can be customized by using delegates.
Proxies attach to models and allow for sorting/filtering.

Used for:
- GroupModel makes turns a Group into a QAbstractItemModel
- CheckableListProxy adds checkboxes and a (Check All) row to any model. Used for filters.
- The Model outliner is based on ModelGroupModel. ModelGroupModel is a tree model with node names, types, and active years

## Our Model View

Narratives have a custom MVP-like structure. I played with a bunch of different designs... they all have a lot of repetition, but this is what we ended up with:

Used for:
- Group is the base class for models
- HorizontalScrollBox is a view
- GroupProxy is a generic proxy for a Group. ERFilterSortProxy extends GroupProxy.
- Narratives/Slides/Labels are based on our MVC
- Embedded Resources

TODO:
- I recently removed osg serializers -> our stuff doesn't need to inherit Group or osg::Node, sooo... there are many options to explore now

### Model

The model is the osg tree. Data nodes like NarrativeSlide emit signals when data changes. Commands operate on the data and allow for undo/redo of changes. There are generic template commands in Commands.h.

Group operations such as adding/removing items are handled by the Group class. There are generic undoable commands for adding/removing nodes from a group (see Group::NewNodeCommand<T>).

### Control

The controllers takes all edit signals from the scroll boxes, buttons, window actions, etc. and maps them to commands and command macros.

The narrative control also keeps track of the current narrative and current slide. It relinks the scroll boxes and canvas when the active narrative/slide change.

Sorry these things are massive, complicated, and hard to test -__-

### Views

Views are the gui parts. They listen to data changes and change the gui accordingly.

HorizontalScrollBox is a view of Group. The connection is made with setGroup. sNew() and sDelete() signals cause the box to create new items with the virtual method createItem. The narrative box creates NarrativeScrollItems and the slide box creates SlideScrollItems.

NarrativeScrollItem is a view of a single narrative. The connection is made with setNarrative(Narrative2*). The item listens to data changed signals like sAuthorChanged(string) and reacts by changing the gui.

Edit signals often don't directly manipulate the gui. For example, Edit Duration on a SlideScrollItem -> Edit Duration Signal from SlideScrollBox -> Edit Duration creates a dialog, returns from dialog and creates SetDurationCommand (Control) -> setDuration (NarrativeSlide), this is where the actual data changes -> sDurationChanged signal (NarrativeSlide) -> duration label setText (SlideScrollItem)


## Considerations

### Why model/view/something?

- Technically the goal is to make it easier to test... but we don't have unit tests so that doesn't really matter.
- I mostly wanted a flow that makes sense, isn't too repetitive, and has some separation of gui and data. Also I had to fit in undo/redo and multi operations

### Why do we have our own?

- Multiple types (Narratives/Slides/Labels) in a fixed hierarchy. For a Qt Model View this means dynamic casting all over the place.
- The integer indexing for the Qt model means big switch tables. (3 types) x (k data items) x (n data operations) = big code mess of switch cases.
- Qt views are not necessary. We don't really need the default views for any of the narrative data. A tree view might be nice but it's unnecessary.
- Undo/redo is a pain with Qt models

### Why does the control exist? Edit signals could pass directly from items to data

- Originally the reason for the control was because the little +/- buttons don't know anything about Narratives or selections. There had to be some class that links buttons to the scroll boxes and to the data.
- Set operations are easier from a bigger control class. Macros make it so that the model can remain free of set operations, hence simpler.
- Selection undo/redo is (probably) easier from a bigger control class.
- The need for an owner of current narrative, current slide, selection level (narratives or slides or labels), selection sets.

TODO: explain this

- Narratives/Slides/Labels
- Embedded Resources


## Disconnecting

Maintaining consistency between gui and data is hard.

Suppose a gui thing is listening to changes in Joe*. The model removes Joe* and emits removed index 3. The gui has to somehow disconnect Joe*, so it would have to maintain a map index->ptr to cleanup. Alternatively you can easily disconnect on aboutToRemove(3).

1. aboutToRemove/removed
2. removed(*)
3. keep smart pointer reference

## Efficient removal/insertion

Suppose you have a scroll box with 1000 items. Select 500 and delete them. If the box refreshes it's layout on every deletion its a O(n^2) problem. Making it an O(n) or O(logn) requires grouping the operations together. I kept changing my mind about how to handle this, first leaving it at n^2, then trying sets, then trying vectors, then going to range removal... so the code is really messy and uses different methods depending on the situation.

Possible Optimizations

1. Set removal {1, 4, 5, 9, 10}.
2. Range removal [1,23], [29,31].
3. Begin/end, queue up all changes, when end() comes in do all updates at once.

Signaling stages

1. view->control, getSelection(T)
    - ScrollBoxes - selectionStack
    - Canvas - set of pointers
2. control->data, insertItems(T) and removeItems(T)
    - insertSet - sorted list of (index, node)
    - insertSetP - set of pointers
    - *removeRange - index, count
    - removeSet - set of indices
    - removeSetP - set of pointers
    - *the other operations use these as a base
3. data->view, itemsInserted(T) and itemsRemoved(T)
    - aboutToRemove - index, count
    - removed - index, count
    - aboutToInsertSet - sorted list of (index, node)
    - insertedSet - //
    - aboutToAddP - set of pointers
    - addedP - //

Qt uses range removal, so we kind of have to do the same in the data->view stage. It's hard to map aboutToRemoveSet to multiple range removals since the Qt model only allows one range removal at a time.