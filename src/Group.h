#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <osg/Group>
#include <QUndoCommand>

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
	virtual void move(const std::vector<std::pair<int, int>>& mapping);
	virtual int indexOf(const osg::Node *node) const; // -1 if not found

signals:
	void sNew(int index); // inserted node at index
	void sDelete(int index); // deleted node at index
	void sSet(int index); // changed (set) node at index
	void sItemsMoved(std::vector<std::pair<int,int>>); // items sorted by .first

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
		MoveNodesCommand(Group *group, std::vector<std::pair<int, int>> mapping, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		void move(std::vector<std::pair<int, int>>); // generic version handles undo and redo
		Group *m_group;
		std::vector<std::pair<int, int>> m_mapping;
	};
};

#endif