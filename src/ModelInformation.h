#ifndef MODELINFORMATION_H
#define MODELINFORMATION_H

#include <string>
#include <osg/Matrix>

class ModelInformation : public osg::Node {
public:
	ModelInformation()
		: Branding_On(true),
		Loop_On(true),
		Collision_On(true),
		NBar_On(true),
		EBar_On(true),
		SR_Customized(false),
		SR_Selection(0), 
		Customized_SR_X(0), 
		Customized_SR_Y(0), 
		Lock_ExpDate(false), 
		Lock_CCreator(false), 
		Lock_Restrict2Current(false), 
		Lock_RestrictOutput(false), 
		Field_of_View(55), 
		Eye_Height(1.5), 
		Near_Clip(1.0), 
		Far_Clip(5000.0), 
		Flight_Acc_Factor(6.0)
	{
		Initial_Matrix.makeIdentity();//initially identity matrix
	};
	ModelInformation(const ModelInformation&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) 
		: Branding_On(true),
		Loop_On(true),
		Collision_On(true),
		NBar_On(true),
		EBar_On(true),
		SR_Customized(false),
		SR_Selection(0),
		Customized_SR_X(0),
		Customized_SR_Y(0),
		Lock_ExpDate(false),
		Lock_CCreator(false),
		Lock_Restrict2Current(false),
		Lock_RestrictOutput(false),
		Field_of_View(55),
		Eye_Height(1.5),
		Near_Clip(1.0),
		Far_Clip(5000.0),
		Flight_Acc_Factor(6.0)
	{
	};
	virtual ~ModelInformation(){};
	
	META_Node(, ModelInformation)

	const std::string& getModelName() const { 
		return Model_Name;
	}

    void setModelName(const std::string& modelname) {
		Model_Name = modelname;
	}
	
	const std::string& getProjectDate() const {
		return Project_Date; 
	}
    void setProjectDate(const std::string& pd) {
		Project_Date = pd; 
		
	}

	const std::string& getReleaseDateVersion() const { 
		return Release_Date_Version;
	}
    void setReleaseDateVersion(const std::string& rd) { 
		Release_Date_Version = rd; 
	}

	const std::string& getPrimaryAuthors() const { 
		return Primary_Authors; 
	}
    void setPrimaryAuthors(const std::string& pa) { 
		Primary_Authors = pa;
	}

	const std::string& getContributors() const { 
		return Contributors; 
	}
    void setContributors(const std::string& contribu) { 
		Contributors = contribu; 
	}

	const std::string& getPlaceofPublication() const { 
		return Place_of_Publication; 
	}
    void setPlaceofPublication(const std::string& place) { 
		Place_of_Publication = place; 
	}

	const std::string& getAccessInformationURL() const { 
		return Access_Information_URL; 
	}
    void setAccessInformationURL(const std::string& accessinformation) { 
		Access_Information_URL = accessinformation; 
	}

	bool getBrandingOn() const{return Branding_On;}
	void setBrandingOn(bool bo){ Branding_On = bo;}	
	
	bool getLoopOn() const{return Loop_On;}
	void setLoopOn(bool lo){ Loop_On = lo;}	

	bool getCollisionOn() const{return Collision_On;}
	void setCollisionOn(bool co){ Collision_On = co;}	

	bool getNBarOn() const{return NBar_On;}
	void setNBarOn(bool nbo){ NBar_On = nbo;}	

	bool getEBarOn() const{return EBar_On;}
	void setEBarOn(bool ebo){ EBar_On = ebo;}	

	bool getSRCustomized() const{return SR_Customized;}
	void setSRCustomized(bool src){ SR_Customized = src;}	

	bool getLockExpDate() const{return Lock_ExpDate;}
	void setLockExpDate(bool ld){ Lock_ExpDate = ld;}	

	bool getLockCCreator() const{return Lock_CCreator;}
	void setLockCCreator(bool src){ Lock_CCreator = src;}	

	bool getLockRestrict2Current() const{return Lock_Restrict2Current;}
	void setLockRestrict2Current(bool src){ Lock_Restrict2Current = src;}	

	bool getLockRestrictOutput() const{return Lock_RestrictOutput;}
	void setLockRestrictOutput(bool src){ Lock_RestrictOutput = src;}	

	int getSRSelection() const{return SR_Selection;}
	void setSRSelection(int srs){SR_Selection = srs;}

	int getCustomizedSRX() const{return Customized_SR_X;}
	void setCustomizedSRX(int csrx){Customized_SR_X = csrx;}
	
	int getCustomizedSRY() const{return Customized_SR_Y;}
	void setCustomizedSRY(int csry){Customized_SR_Y = csry;}

	void setInitialMatrix(const osg::Matrixd& matrix) 
	{
		Initial_Matrix = matrix; 	
	}
	const osg::Matrixd& getInitialMatrix() const { return Initial_Matrix; }

	const std::string& getExpireDate() const { 
		return Expire_Date;
	}
    void setExpireDate(const std::string& ed) { 
		Expire_Date = ed; 
	}

	float getFieldView() const {return Field_of_View;}
	void setFieldView(float fov){Field_of_View = fov;}

	float getEyeHeight() const {return Eye_Height;}
	void setEyeHeight(float eh){Eye_Height = eh;}

	float getNearClip() const {return Near_Clip;}
	void setNearClip(float nc){Near_Clip = nc;}

	float getFarClip() const {return Far_Clip;}
	void setFarClip(float fc){Far_Clip = fc;}

	float getFlightAccFactor() const {return Flight_Acc_Factor;}
	void setFlightAccFactor(float fac){Flight_Acc_Factor = fac;}

public:

	std::string Model_Name;
	std::string Release_Date_Version;
	std::string Project_Date;
	std::string Primary_Authors;
	std::string Contributors;
	std::string Place_of_Publication;
	std::string Access_Information_URL;
	bool Branding_On;
	bool Loop_On;
	bool Collision_On;
	bool NBar_On; //N bar open at launch
	bool EBar_On;
	bool SR_Customized;
	osg::Matrixd Initial_Matrix;
	int SR_Selection;
	int Customized_SR_X;
	int Customized_SR_Y;

	bool Lock_ExpDate;
	bool Lock_CCreator;
	bool Lock_Restrict2Current;
	bool Lock_RestrictOutput;
	std::string Expire_Date;

	float Field_of_View;
	float Eye_Height;
	float Near_Clip;
	float Far_Clip;
	float Flight_Acc_Factor;
};

#endif