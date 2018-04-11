#include <QDebug>
#include <string>
#include <QListView>
#include <QFileDialog>
#include <QMessageBox>
#include "resources/ERDialog.h"
#include "resources/NewCatDialog.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategoryControl.h"
#include "resources/ECategoryModel.h"
#include "EditDeleteDelegate.h"
#include "GroupModelTemplate.h"

ERDialog::ERDialog(ECategoryControl * category_control, QString current_dir, QWidget * parent)
	: QDialog(parent),
	m_control(category_control),
	m_current_dir(current_dir)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	for (uint i = 0; i < sizeof(EResource::CopyrightStrings) / sizeof(char*); i++) {
		ui.licensing->addItem(EResource::CopyrightStrings[i]);
	}
	ui.licensing->setCurrentIndex(EResource::UNSPECIFIED);

	init(nullptr);

	connect(ui.choose, &QPushButton::clicked, this, &ERDialog::chooseFile);

	// only one call to onTypeChange occurs when toggles happen
	auto ontoggletype = [this](bool x) { if (x) this->onTypeChange(); };
	connect(ui.url, &QRadioButton::toggled, this, ontoggletype);
	connect(ui.file, &QRadioButton::toggled, this, ontoggletype);
	connect(ui.annotation, &QRadioButton::toggled, this, ontoggletype);
	connect(ui.relative, &QAbstractButton::toggled, this, &ERDialog::setRelative);
	connect(ui.path, &QLineEdit::textEdited, this, &ERDialog::setPath);

	auto ontoggleactive = [this](bool x) { if (x) this->onActivationChange(); };
	connect(ui.global, &QRadioButton::toggled, this, ontoggleactive);
	connect(ui.local, &QRadioButton::toggled, this, ontoggleactive);

	onActivationChange();
	onTypeChange();

	ui.year_lower->setValue(0);
	ui.year_upper->setValue(0);
	ui.annotation->setChecked(true);
	ui.radius->setValue(10);

	// category stuff
	m_category_view = new QListView(this);
	m_category_view->setModel(m_control->categoryModel());
	m_category_delegate = new EditDeleteDelegate(this);
	m_category_view->setItemDelegate(m_category_delegate);
	ui.categories->setMouseTracking(true);
	ui.categories->setModel(m_control->categoryModel());
	ui.categories->setView(m_category_view);

	connect(ui.addnew, &QPushButton::pressed, this,
		[this]() {
		auto cat = m_control->execNewCategory();
		setCategory(cat.get());
	});
	connect(m_category_delegate, &EditDeleteDelegate::sEdit, this,
		[this](QAbstractItemModel *model, const QModelIndex &index) {
		ECategory *cat = m_control->execEditCategory(model, index);
		ui.categories->setCurrentIndex(index.row());
	});
	connect(m_category_delegate, &EditDeleteDelegate::sDelete, m_control, &ECategoryControl::execDeleteCategory);
}

void ERDialog::init(const EResource * er)
{
	if (er == nullptr) { // Defaults
		ui.title->setText("Untitled");
		ui.description->setText("");
		ui.authors->setText("");
		ui.authors->setText("");
		// make all of the other fields just copy the previous?
		return;
	}

	ui.title->setText(QString::fromStdString(er->getResourceName()));
	ui.description->setText(QString::fromStdString(er->getResourceDescription()));
	ui.authors->setText(QString::fromStdString(er->getAuthor()));
	ui.year_lower->setValue(er->getMinYear());
	ui.year_upper->setValue(er->getMaxYear());
	ui.radius->setValue(er->getLocalRange());
	ui.licensing->setCurrentIndex(er->getCopyright());

	if (er->getGlobal()) ui.global->setChecked(true);
	else ui.local->setChecked(true);

	ui.autolaunch->setChecked(er->getAutoLaunch());

	switch (er->getERType()) {
	case EResource::FILE:
		ui.file->setChecked(true);
		break;
	case EResource::ANNOTATION:
		ui.annotation->setChecked(true);
		break;
	case EResource::URL:
		ui.url->setChecked(true);
		break;
	}
	ui.path->setText(QString::fromStdString(er->getResourcePath()));
	checkRelative();
	
	// setup the category selection
	setCategory(er->category());
}

std::string ERDialog::getTitle() const
{
	return ui.title->text().toStdString();
}

std::string ERDialog::getDescription() const
{
	return ui.description->toPlainText().toStdString();
}

std::string ERDialog::getAuthor() const
{
	return ui.authors->toPlainText().toStdString();
}

std::string ERDialog::getPath() const
{
	if (ui.annotation->isChecked()) return "";
	return ui.path->text().toStdString();
}

EResource::Copyright ERDialog::getCopyright() const
{
	return (EResource::Copyright)ui.licensing->currentIndex();
}

int ERDialog::getMinYear() const
{
	return ui.year_lower->value();
}

int ERDialog::getMaxYear() const
{
	return ui.year_upper->value();
}

bool ERDialog::getGlobal() const
{
	return ui.global->isChecked();
}

bool ERDialog::getReposition() const
{
	return ui.on->isChecked();
}

bool ERDialog::getAutoLaunch() const
{
	return ui.on_2->isChecked() && getERType() != EResource::ANNOTATION;
}

float ERDialog::getLocalRange() const
{
	if (ui.global->isChecked()) {
		return 0;
	}
	return ui.radius->value();
}

EResource::ERType ERDialog::getERType() const
{
	if (ui.file->isChecked()) {
		return EResource::FILE;
	}
	else if (ui.annotation->isChecked()) {
		return EResource::ANNOTATION;
	}
	else {
		return EResource::URL;
	}
}

ECategory * ERDialog::categoryAt(int i) const
{
	ECategory *cat = TGroupModel<ECategory>::get(m_control->categoryModel(), i);
	return cat;
}

ECategory *ERDialog::getCategory() const
{
	return categoryAt(ui.categories->currentIndex());
}

std::shared_ptr<ECategory> ERDialog::categoryShared() const
{
	int row = ui.categories->currentIndex();
	QModelIndex index = ui.categories->model()->index(row, 0);
	return m_control->categoryModel()->getShared(index);
}

void ERDialog::setCategory(const ECategory * cat)
{
	if (!cat) return;
	for (int i = 0; i < ui.categories->count(); i++) {
		if (cat == categoryAt(i)) {
			ui.categories->setCurrentIndex(i);
			break;
		}
	}
}

void ERDialog::chooseFile()
{
	QFileDialog dlg;

	QString open_dir;
	if (ui.path->text() == "") {
		open_dir = m_current_dir;
	}
	else {
		QString abs_path;
		// if relative, then build the abs path, take the directory
		if (QDir::isRelativePath(ui.path->text())) {
			abs_path = m_current_dir + "/" + ui.path->text();
		}
		else {
			abs_path = ui.path->text();
		}
		open_dir = QFileInfo(abs_path).absoluteDir().path();
	}
	QString file = dlg.getOpenFileName(this, QString(), open_dir);
	if (file == "") return;

	QString new_path;
	if (ui.relative->isChecked()) {
		QDir dir(m_current_dir);
		new_path = dir.relativeFilePath(file);
	}
	else {
		new_path = file;
	}
	ui.path->setText(new_path);
}

void ERDialog::setPath(const QString & s)
{
	checkRelative();
}

void ERDialog::setRelative(bool relative)
{
	QString path = ui.path->text();

	// is the path already relative/absolute?
	if (QDir::isRelativePath(path) == relative) {
		// don't do anything
		return;
	}

	QString new_path;
	if (relative) {
		// make this relative
		new_path = QDir(m_current_dir).relativeFilePath(path);
	}
	else {
		// want to make this absolute
		// canonical doesn't work for files that don't exist
		// QFileInfo(m_current_dir + "/" + path).canonicalFilePath();
		new_path = QDir::cleanPath(m_current_dir + "/" + path);
	}
	ui.path->setText(new_path);
	//checkRelative(); // possible infinite loop, should still work w/o this
}

void ERDialog::checkRelative()
{
	if (ui.file->isChecked()) {
		bool rel = QDir::isRelativePath(ui.path->text());
		ui.relative->setChecked(rel);
	}
}

void ERDialog::onTypeChange()
{
	if (ui.file->isChecked()) {
		ui.path->setEnabled(true);
		ui.choose->setEnabled(true);
		ui.relative->setEnabled(true);
		checkRelative();
	}
	else if (ui.annotation->isChecked()) {
		ui.path->setEnabled(false);
		ui.choose->setEnabled(false);
		ui.relative->setEnabled(false);
	}
	else if (ui.url->isChecked()) {
		ui.path->setEnabled(true);
		ui.choose->setEnabled(false);
		ui.relative->setEnabled(false);
	}
}

void ERDialog::onActivationChange()
{
	if (ui.global->isChecked()) {
		ui.radius->setEnabled(false);
	}
	else if (ui.local->isChecked()) {
		ui.radius->setEnabled(true);
	}
}
