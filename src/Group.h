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
};



#endif