
##Qt Model View

has its own ModelView framework
It's organized in tables, with columns and rows
Hierarchy is possible with parents


Used for:
- Models


##Our Model View

Narratives have a custom MVP-like structure

Why do our own?
reasons
- multiple types Narratives/Slides/Labels at fixed places. This means dynamic casting all over the place.
- The indexing meant big switch tables. Multiple types * switch tables for each data item * data/setData/addItem/removeItem = big code mess.
- It's possible to make Item classes for each data class, ex. NarrativeItem which maps the indexes to data... but maybe it's too much work?
- overkill? We don't really need the default views for any of the narrative data. A tree view might be nice but it's unnecessary.
- undo/redo is a pain with Qt models

What is our own?
I played with a bunch of different designs... they pretty much all have a lot of repetition

TODO: explain this

- Narratives/Slides/Labels
- Embedded Resources