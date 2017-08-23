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

signals:
	void sNew(int);
	void sDelete(int);
	void sItemsMoved(std::vector<std::pair<int,int>>); // items sorted by .first
	
public: // COMMANDS
	template <typename T>
	class NewNodeCommand : public QUndoCommand {
	public:
		NewNodeCommand(
			Group *group,
			int index,
			QUndoCommand *parent = nullptr)
			: QUndoCommand(parent),
			m_group(group),
			m_index(index)
		{
			m_node = new T;
		}
		void undo() {
			m_group->removeChild(m_index);
			m_group->sDelete(m_index);
		}
		void redo() {
			m_group->insertChild(m_index, m_node);
			m_group->sNew(m_index);
		}
		T *getNode() {
			return m_node;
		}
	private:
		Group *m_group;
		osg::ref_ptr<T> m_node;
		int m_index;
	};

	template <typename T>
	class DeleteNodeCommand : public QUndoCommand {
	public:
		DeleteNodeCommand(
			Group *group,
			int index,
			QUndoCommand *parent = nullptr)
			: QUndoCommand(parent),
			m_group(group),
			m_index(index)
		{
			m_node = dynamic_cast<T*>(group->getChild(index));
		}
		void undo() {
			m_group->insertChild(m_index, m_node);
			m_group->sNew(m_index);
		}
		void redo() {
			m_group->removeChild(m_index);
			m_group->sDelete(m_index);
		}
	private:
		Group *m_group;
		osg::ref_ptr<T> m_node;
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