/*
 * NarrativeReference.h
 *
 *  Created on: Mar 29, 2011
 *      Author: eduardo
 */

#ifndef NARRATIVEREFERENCE_H_
#define NARRATIVEREFERENCE_H_

#include "narrative/Narrative.h"

class NarrativeReference: public osg::Node
{
public:
    NarrativeReference();
    virtual ~NarrativeReference();

    virtual Object* cloneType() const { return new NarrativeReference(); }
    //virtual Object* clone(const osg::CopyOp& copyop) const { return new Narrative(*this, copyop); }
    virtual bool isSameKindAs(const Object* obj) const { return dynamic_cast<const NarrativeReference*>(obj)!=NULL; }
    virtual const char* libraryName() const { return ""; }
    virtual const char* className() const { return "NarrativeReference"; }

    const std::string& getFilename() const { return m_filename; }
    void setFilename(const std::string& filename) { m_filename = filename; }

    Narrative* getNarrative() const { return m_narrative.get(); }
    void setNarrative(Narrative* narrative) { m_narrative = narrative; }

protected:
    std::string m_filename;
    osg::ref_ptr<Narrative> m_narrative;
};

#endif /* NARRATIVEREFERENCE_H_ */
