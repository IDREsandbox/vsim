
# Embedded Resources

[diagram](https://drive.google.com/file/d/1Xp8H5fiNRlCFlpdBbqbmOZdcxKpqly4w/view?usp=sharing)

## how things work

GroupSignals has a generic interface
 sAboutToInsert(std::vector<int> sorted)
 sInserted...
and a bunch of virtuals

ERFilterSortProxy extends that interface for EResources and provides filtering/sorting operations on top of an existing TGroup<EResource>. The common interface allows ERFilterSortProxies to be chained together.


all resources -> main proxy -> local proxy
                            -> global proxy

The local box views the local proxy. The global box views the global proxy. The all box views the all proxy.

I was pretty much copying Qt's model.

## issues, limitations

I think the group interface is too complicated, not even worth it. I was trying really hard to avoid O(n^2) insertions, deletions, moves.

Insertions/deletions on the proxy itself don't work (too complicated). The ER Control takes the raw pointers and works directly on the base EResourceGroup.

## speed

The current implementation is super inefficient. I started off trying to do fancy stuff, but there were occasional crashes in debug mode. I tested out a dumb simple solution and it seems good enough so I left it. There is a lot of leftover gargabe code though.

On every frame, each scrollbox just refreshes everything.
ERControl::onUpdate() -> position poke -> reload2
                      -> emit sEdited on group -> recursive rescan on each proxy -> rescan -> reload2

reload2 is kind of crazy. It takes two vectors of indices and figures out insertions/removals/moves to turn one into the other.

result: 1ms with 600 resources, 4ms when searching complicated text

improvements: 