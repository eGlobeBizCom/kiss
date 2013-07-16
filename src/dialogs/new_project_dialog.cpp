#include "new_project_dialog.hpp"

#include <QFileDialog>
#include <QSettings>
#include <QDebug>

#define SETTINGS_GROUP "New Project Wizard"
#define SETTINGS_KEY "Save Location"

using namespace Kiss::Dialog;

NewProjectDialog::NewProjectDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	ui_projectName->selectAll();
		
	QSettings settings;
	settings.beginGroup(SETTINGS_GROUP);
	m_savePath = settings.value(SETTINGS_KEY, QDir::homePath()).toString();
	settings.endGroup();
	updateSaveLocation();
}

QString NewProjectDialog::projectName() const
{
	return ui_projectName->text();
}

QString NewProjectDialog::saveLocation() const
{
	return ui_saveLocation->text();
}

void NewProjectDialog::on_ui_browse_clicked()
{
	m_savePath = QFileDialog::getExistingDirectory(this, tr("Choose a Save Location"));
	QSettings settings;
	settings.beginGroup(SETTINGS_GROUP);
	settings.setValue(SETTINGS_KEY, m_savePath);
	settings.endGroup();
	updateSaveLocation();
}

void NewProjectDialog::on_ui_projectName_textChanged(const QString&)
{
	updateSaveLocation();
}

void NewProjectDialog::updateSaveLocation()
{
	ui_saveLocation->setText(m_savePath + "/" + ui_projectName->text());
}