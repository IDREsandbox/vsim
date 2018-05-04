#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoStack>

namespace Command {
	// you can use these like flags
	enum When {
		ON_UNDO = 1,
		ON_REDO = 2,
		ON_BOTH = ON_UNDO | ON_REDO
	};
}

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

#endif