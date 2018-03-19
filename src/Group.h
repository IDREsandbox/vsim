#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <osg/Group>
#include <QUndoCommand>

// Group is a collection of nodes which emits signals
//  on every change
// If dealing with single indices use insert/remove
// Dealing with sets of indices use insertSet/removeSet
// Dealing with sets of nodes use addP/removeP
class Group : public QObject, public osg::Group {
	Q_OBJECT
public:
	Group() {};
	Group(const Group& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {};
	META_Node(, Group)

	// Use this instead of the osg one. For some reason the osg one isn't virtual or const
	virtual osg::Node *child(unsigned int index) const;
	virtual bool addChild(osg::Node *child) override;
	virtual bool insertChild(unsigned int index, osg::Node *child) override;
	virtual bool removeChildren(unsigned int index, unsigned int numChildrenToRemove) override;
	virtual bool setChild(unsigned int index, Node *child) override;

	virtual void move(const std::vector<std::pair<size_t, size_t>>& mapping);

	virtual int indexOf(const osg::Node *node) const; // -1 if not found

	virtual void clear();

	// index based, single
signals:
	void sNew(int index); // inserted node at index
	void sDelete(int index); // deleted node at index
	void sSet(int index); // changed (set) node at index
	void sItemsMoved(std::vector<std::pair<size_t, size_t>>); // items sorted by .first

	void sEdited(const std::set<int> &edited); // for proxy/big listeners

	void sBeginReset();
	void sReset();

public: // continuous *
	virtual bool insert(size_t index, const std::vector<osg::Node*> &nodes);
	virtual bool remove(size_t index, size_t count);
signals:
	void sAboutToInsert(size_t index, const std::vector<osg::Node*> &nodes);
	void sInserted(size_t index, const std::vector<osg::Node*> &nodes);
	void sAboutToRemove(size_t index, size_t count);
	void sRemoved(size_t index, size_t count);

public: // index based, multiple
	virtual void insertChildrenSet(const std::vector<std::pair<size_t, osg::Node*>> &children);
	virtual void removeChildrenSet(const std::vector<size_t> &children);
signals:
	void sAboutToInsertSet(const std::vector<std::pair<size_t, osg::Node*>> &);
	void sInsertedSet(const std::vector<std::pair<size_t, osg::Node*>> &);
	void sAboutToRemoveSet(const std::vector<size_t> &);
	void sRemovedSet(const std::vector<size_t> &);

public: // pointer based, multiple
	virtual void addChildrenP(const std::set<osg::Node*> &children);
	virtual void removeChildrenP(const std::set<osg::Node*> &children);
signals:
	void sAboutToAddP(const std::set<osg::Node*> &);
	void sAddedP(const std::set<osg::Node*> &);
	void sAboutToRemoveP(const std::set<osg::Node*> &nodes);
	void sRemovedP(const std::set<osg::Node*> &nodes);

public: // COMMANDS
	// Adds an already created node to a group
	// if index < 0 then it adds it to the end
	class AddNodeCommand : public QUndoCommand {
	public:
		AddNodeCommand(
			Group *group,
			osg::Node *node,
			int index = -1,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group *m_group;
		osg::ref_ptr<osg::Node> m_node;
		int m_index;
	};

	// Deletes a node at index
	class DeleteNodeCommand : public QUndoCommand {
	public:
		DeleteNodeCommand(
			Group *group,
			int index,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group *m_group;
		osg::ref_ptr<osg::Node> m_node;
		int m_index;
	};

	class MoveNodesCommand : public QUndoCommand {
	public:
		// mapping .first is from, .second is to
		MoveNodesCommand(Group *group, const std::vector<std::pair<size_t, size_t>> &mapping, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		Group *m_group;
		std::vector<std::pair<size_t, size_t>> m_mapping;
	};

	// index based, multiple
	class InsertSetCommand : public QUndoCommand {
	public:
		InsertSetCommand(
			Group *group,
			const std::map<int, osg::Node*> &nodes,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group *m_group;
		std::map<int, osg::ref_ptr<osg::Node>> m_nodes;
	};

	// index based, multiple
	class RemoveSetCommand : public QUndoCommand {
	public:
		RemoveSetCommand(
			Group *group,
			const std::set<int> &nodes,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group * m_group;
		std::map<int, osg::ref_ptr<osg::Node>> m_nodes;
	};

	// pointer based, multiple
	class AddNodesPCommand : public QUndoCommand {
	public:
		AddNodesPCommand(
			Group *group,
			const std::set<osg::Node*> &nodes,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group * m_group;
		std::set<osg::ref_ptr<osg::Node>> m_nodes;
	};

	// pointer based, multiple
	class RemoveNodesPCommand : public QUndoCommand {
	public:
		RemoveNodesPCommand(
			Group *group,
			const std::set<osg::Node*> &nodes,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group *m_group;
		std::set<osg::ref_ptr<osg::Node>> m_nodes;
	};

	// causes sEdited to be emitted on undo/redo
	class EditCommand : public QUndoCommand {
	public:
		EditCommand(
			Group *group,
			const std::set<int> &nodes,
			QUndoCommand *parent = nullptr);
		virtual void undo() override;
		virtual void redo() override;
	private:
		Group *m_group;
		std::set<int> m_nodes;
	};
};

#endif