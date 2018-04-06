# .vsim format

See (serialization)[./serialization.md] for pros/cons/decisions

## New .vsim Format
- uses Google Flatbuffers for app data
    - Narratives and slides
	- Embedded Resources
	- model information, model metadata
- uses OSG binary format (.osgb) for model data, we aren't restricted to this

Flatbuffer schemas are in .fbs files, they're pretty self-documenting.

file format
```
    vsim header -              [[4: vsim][4: unused]]
    size prefixed flatbuffer - [[4: size][flatbuffer]] 
    model data -               [[model 1][model 2]...]
```

vsim header -
```
    V  S  I  M  x  x  x  x
    56 53 49 4d xx xx xx xx
```

### Issues

One issue with flatbuffers is that they don't support files > 2GB, so we hack
around this by dumping the model binary after the flatbuffer. To know where
the flatbuffer ends and the model starts, we use a size-prefixed flatbuffer.
Model size/format information is kept in the flatbuffer.

 it's hard to figure out model size before writing (see vsim.fbs, Model)
 solutions:
- use mutable buffers & mmap, go back and update sizes after writing out models
- could write model data to string in memory first
    - currently used, but we might have memory issues later
- could write size-prefixed blocks
    - maybe?
 for now be lazy, no changes

## Old .vsim Format
- based on OSG serializers
- everything is a node under the root
- extract nodes on load
- reinsert nodes on export

```
    "vsim" osg::Group (whatever model was loaded is the root)
        Children {
            "VsimBrandingOverlay" ::VSCanvas
            ModelInformation ::ModelInformation
            Model2
            Model3
            Narrative1 ::Narrative
            Narrative2 ::Narrative
                NarrativeNodes, user serializer {
                    Node1 ::NarrativeNode (osg::Group)
                        mostly same as ours
                        Children {
                            "overlaycanvas" ::VSCanvas (osgNewWidget::Canvas osgNewWidget::Widget osg::Geode)
                                all of the widget stuff, x/y/w/h whatever
                                Group osg::Group
                                    Children {
                                        widget1 ::VSWidget
                                        widget2 ::VSLabel
                                            Position osg::Vec2f
                                            Size osg::Vec2f
                                            BgGeometry
                                            BorderGeometry
                                            bunch of other random stuff
                                    }
                        }
                    Transition 1
                        same as ours
                    Node 2
                    Transition 2
                }
                Selection, user serializer
                Name
                Author
                Description
        }
    }
```

Old .nar Format (osgb)	
Narrative1 ::Narrative {
	. . .
}

osg/obj/fbx/dae/skt/flt/etc?
 this is a raw model, do importing instead
 