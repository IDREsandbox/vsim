#ifndef GROUPPROXY_H
#define GROUPPROXY_H

#include "Group.h"

// 
class GroupProxy : public Group {
	Q_OBJECT
public:
	GroupProxy();
	GroupProxy(Group *base);
	GroupProxy(const Group& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {};
	META_Node(, GroupProxy);

	Group *getBase() const;
	virtual void setBase(Group *base);

	int mapToBase(int i);
	//int mapFromBase(int i);

	void setMapToBase(const std::vector<int> &mapping);

	// overrides
	virtual bool insertChild (unsigned int index, Node *child) override;
	virtual bool removeChildren (unsigned int index, unsigned int numChildrenToRemove) override;
	virtual osg::Node *child(unsigned int index) override;
	virtual unsigned int getNumChildren() const override;

private:
	osg::ref_ptr<Group> m_base;
	std::vector<int> m_map_to_base;
};

#endif