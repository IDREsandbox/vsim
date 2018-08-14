#ifndef MODELINFORMATIONDIALOG_H
#define MODELINFORMATIONDIALOG_H

#include <QDialog>
#include <string>
#include <stdint.h>
#include <memory>

namespace Ui { class modelInformationDialog; }
namespace VSim { namespace FlatBuffers { struct ModelInformationT; }}

class ModelInformationDialog : public QDialog {
	Q_OBJECT
public:
	ModelInformationDialog(const VSim::FlatBuffers::ModelInformationT *data,
		QWidget *parent = Q_NULLPTR);
	~ModelInformationDialog();

	VSim::FlatBuffers::ModelInformationT getData();

	bool readOnly() const;
	void setReadOnly(bool ro);

private:
	std::unique_ptr<Ui::modelInformationDialog> ui;
	bool m_read_only;
};

#endif