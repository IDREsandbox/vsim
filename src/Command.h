#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoStack>

// Generic undoable commands utilizing getters, setters, signals

// ModifyCommand is for changing a single value.
// When you redo this calls set(newvalue).
// When you undo this calls set(oldvalue).
// TODO should a signal be emitted after the operation? or should the get/set handle it?
//   for now the setters do the signalling
// TODO description text?
// just a little bit of C++ black magic allows us to handle const std::string&, const osg::Matrixd&, etc
template <typename C, typename T>
class ModifyCommand : public QUndoCommand {
public:
	ModifyCommand(
		T(C::*getter)() const,//pointer to getter
		void(C::*setter)(T),//pointer to setter
		//void(C::*signal)(T),//pointer to signal
		T value,
		C *obj,
		QUndoCommand *parent = nullptr
	)
		: QUndoCommand(parent),
		m_getter(getter),
		m_setter(setter),
		//m_signal(signal),
		new_value(value),
		m_obj(obj)
	{
		old_value = (obj->*m_getter)();
		//qDebug() << m_getter << m_setter << new_value << old_value << m_obj;
	}
	void undo() {
		(m_obj->*m_setter)(old_value);
	}
	void redo() {
		(m_obj->*m_setter)(new_value);
	}
private:
	T(C::*m_getter)()const;
	void(C::*m_setter)(T);
	//void(C::*m_signal)(T);
	std::remove_cv_t<std::remove_reference_t<T>> new_value;
	std::remove_cv_t<std::remove_reference_t<T>> old_value;
	C *m_obj;
};

// Generic new/delete commands for osg::Group
// NewNodeCommand<group class, child class>
template <typename C, typename T>
class NewNodeCommand : public QUndoCommand {
public:
	NewNodeCommand(
		void(C::*new_signal)(int),
		void(C::*delete_signal)(int),
		C *group,
		int index,
		QUndoCommand *parent = nullptr)
		: QUndoCommand(parent),
		m_new_signal(new_signal),
		m_delete_signal(delete_signal),
		m_group(group),
		m_index(index)
	{
		m_node = new T;
	}
	void undo() {
		m_group->removeChild(m_index);
		(m_group->*m_delete_signal)(m_index);
	}
	void redo() {
		m_group->insertChild(m_index, m_node);
		(m_group->*m_new_signal)(m_index);
	}
private:
	void(C::*m_new_signal)(int);
	void(C::*m_delete_signal)(int);
	C *m_group;
	osg::ref_ptr<T> m_node;
	int m_index;
};

template <typename C, typename T>
class DeleteNodeCommand : public QUndoCommand {
public:
	DeleteNodeCommand(
		void(C::*new_signal)(int),
		void(C::*delete_signal)(int),
		C *group,
		int index,
		QUndoCommand *parent = nullptr)
		: m_new_signal(new_signal),
		m_delete_signal(delete_signal),
		m_group(group),
		m_index(index) 
	{
		m_node = dynamic_cast<T*>(group->getChild(index));
	}
	void undo() {
		m_group->insertChild(m_index, m_node);
		(m_group->*m_new_signal)(m_index);
	}
	void redo() {
		m_group->removeChild(m_index);
		(m_group->*m_delete_signal)(m_index);
	}
private:
	void(C::*m_new_signal)(int);
	void(C::*m_delete_signal)(int);
	C *m_group;
	osg::ref_ptr<T> m_node;
	int m_index;
};
#endif