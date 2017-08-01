#ifndef NARRATIVEMODEL_H
#define NARRATIVEMODEL_H

#include <QAbstractItemModel>
#include <QUndoStack>

#include <osg/Group>

#include "narrative/Narrative2.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabels.h"

class AddCommand;
class RemoveCommand;
class EditCommand;

class NarrativeModel : public QAbstractItemModel {
	Q_OBJECT
	friend AddCommand;
	friend RemoveCommand;
	friend EditCommand;
public:

	NarrativeModel(QObject *parent, osg::Group *narratives, QUndoStack *stack);

	// This model involves 3 different types - Narratives, Slides, and Labels
	//   Currently type differences are handled with big switches (see type
	//   switch methods). A more extensible approach would to have ::VSimNode
	//   with all of these operations handled through virtual functions (TODO)

	// QAbstractItemModel overrides
	QVariant data(const QModelIndex &index, int role) const override; // type switch
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override; // type switch
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override; // type switch
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()); // type switch
	// The destination index specifies the index of the start of the moved block after the block is moved
	bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	Qt::DropActions supportedDragActions() const;
	Qt::DropActions supportedDropActions() const;

	// Convenience functions
	QModelIndex indexOf(osg::Node *node) const;

	// 
	void debug();

private:
	//private:
	osg::ref_ptr<osg::Group> m_root; // the "Narratives" group
	QUndoStack *m_stack;

};


#endif