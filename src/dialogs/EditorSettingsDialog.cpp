/**************************************************************************
 *  Copyright 2007-2012 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of KISS (Kipr's Instructional Software System).     *
 *                                                                        *
 *  KISS is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  KISS is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with KISS.  Check the LICENSE file in the project root.         *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#include "EditorSettingsDialog.h"
#include "LexerSettingsDialog.h"
#include "LexerFactory.h"

#include <QSettings>
#include <QDebug>

EditorSettingsDialog::EditorSettingsDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	// Set up the Auto Completion Group
	ui_autoCompletionAPISourceCheckBox->connect(ui_autoCompletionEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
	ui_autoCompletionDocumentSourceCheckBox->connect(ui_autoCompletionEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
	ui_autoCompletionThresholdSpinBox->connect(ui_autoCompletionEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
	
	// Set up the auto Indent Group
	ui_autoIndentMaintainStyleRadioButton->connect(ui_autoIndentEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
	ui_autoIndentIntelligentStyleRadioButton->connect(ui_autoIndentEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
	ui_autoIndentWidthSpinBox->connect(ui_autoIndentEnabledCheckBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));

	// Make sure the defaults are saved
	readSettings();
	saveSettings();
}

EditorSettingsDialog::~EditorSettingsDialog() {}

// Show the dialog and save if ok is clicked
int EditorSettingsDialog::exec()
{
	readSettings();
	if(!QDialog::exec()) return QDialog::Rejected;
	saveSettings();
	return QDialog::Accepted;
}

void EditorSettingsDialog::on_ui_lexerSettingsButton_clicked()
{
	LexerSettingsDialog diag(m_lexerSettings);
	if(diag.exec())
		m_lexerSettings = diag.settings();
}

// Save the settings from the dialog
void EditorSettingsDialog::saveSettings()
{
	qWarning() << "Font" << ui_fontBox->currentFont().family();
	QSettings settings;
	settings.beginGroup(EDITOR);
	settings.setValue(BACKGROUND_COLOR, ui_backgroundColorBox->color());
	settings.setValue(FONT, ui_fontBox->currentFont().family());
	settings.setValue(FONT_SIZE, ui_fontSizeSpinBox->value());
	
	settings.beginGroup(LEXER);
	QMap<QString, QColor>::const_iterator i = m_lexerSettings.constBegin();
	while (i != m_lexerSettings.constEnd()) {
		settings.setValue(i.key(), i.value());
		++i;
	}
	Lexer::Settings::ref().setSettings(m_lexerSettings);
	settings.endGroup();
	
	settings.beginGroup(AUTO_COMPLETION);
	settings.setValue(ENABLED, ui_autoCompletionEnabledCheckBox->isChecked());
	settings.setValue(API_SOURCE, ui_autoCompletionAPISourceCheckBox->isChecked());
	settings.setValue(DOC_SOURCE, ui_autoCompletionDocumentSourceCheckBox->isChecked());
	settings.setValue(THRESHOLD, ui_autoCompletionThresholdSpinBox->value());
	settings.endGroup();

	settings.beginGroup(AUTO_INDENT);
	settings.setValue(ENABLED, ui_autoIndentEnabledCheckBox->isChecked());
	if(ui_autoIndentMaintainStyleRadioButton->isChecked()) settings.setValue(STYLE, MAINTAIN);
	else if (ui_autoIndentIntelligentStyleRadioButton->isChecked()) settings.setValue(STYLE, INTELLIGENT);
	settings.setValue(WIDTH, ui_autoIndentWidthSpinBox->value());
	settings.endGroup();

	settings.setValue(CALL_TIPS, ui_callTipsCheckBox->isChecked());
	settings.setValue(BRACE_MATCHING, ui_braceMatchingCheckBox->isChecked());
	settings.setValue(LINE_NUMBERS, ui_marginLineNumbersCheckBox->isChecked());
	settings.setValue(DEBUGGER_ENABLED, ui_debugger->isChecked());
	settings.endGroup();
	settings.sync();
}

// Read settings from the app config, setting defaults if necessary
void EditorSettingsDialog::readSettings()
{
	// Open up the applications settings and look in the editor group
	QSettings settings;
	settings.beginGroup(EDITOR);
	ui_backgroundColorBox->setColor(settings.value(BACKGROUND_COLOR, QColor(255, 255, 255)).value<QColor>());

	// Figure out the font and set it
	#ifdef Q_OS_WIN32
	QString fontString = settings.value(FONT, "Courier New").toString();
	#elif defined(Q_OS_MAC)
	QString fontString = settings.value(FONT, "Monaco").toString();
	#else
	QString fontString = settings.value(FONT, "Monospace").toString();
	#endif
	
	ui_fontBox->setCurrentFont(QFont(fontString));
	
	// Figure out the font size and set the widget
	#ifdef Q_OS_MAC
	ui_fontSizeSpinBox->setValue(settings.value(FONT_SIZE, 12).toInt());
	#else
	ui_fontSizeSpinBox->setValue(settings.value(FONT_SIZE, 10).toInt());
	#endif
	
	// Read the lexer settings
	m_lexerSettings.clear();
	settings.beginGroup(LEXER);
	QStringList keys = settings.childKeys();
	foreach(QString key, keys) m_lexerSettings.insert(key, settings.value(key).value<QColor>());

	settings.endGroup();
	
	// Set the auto completion settings from the application config
	settings.beginGroup(AUTO_COMPLETION);
	ui_autoCompletionEnabledCheckBox->setChecked(settings.value(ENABLED, false).toBool());
	ui_autoCompletionAPISourceCheckBox->setChecked(settings.value(API_SOURCE, false).toBool());
	ui_autoCompletionDocumentSourceCheckBox->setChecked(settings.value(DOC_SOURCE, false).toBool());
	ui_autoCompletionThresholdSpinBox->setValue(settings.value(THRESHOLD, 4).toInt());
	settings.endGroup();

	// Set the auto indent settings from the application config
	settings.beginGroup(AUTO_INDENT);
	ui_autoIndentEnabledCheckBox->setChecked(settings.value(ENABLED, true).toBool());
	if(settings.value(STYLE).toString() == MAINTAIN) ui_autoIndentMaintainStyleRadioButton->setChecked(true);
	else if(settings.value(STYLE).toString() == INTELLIGENT) ui_autoIndentIntelligentStyleRadioButton->setChecked(true);
	ui_autoIndentWidthSpinBox->setValue(settings.value(WIDTH, 4).toInt());
	settings.endGroup();

	// Set the misc options from the application config
	ui_callTipsCheckBox->setChecked(settings.value(CALL_TIPS, true).toBool());
	ui_braceMatchingCheckBox->setChecked(settings.value(BRACE_MATCHING, true).toBool());
	ui_marginLineNumbersCheckBox->setChecked(settings.value(LINE_NUMBERS, true).toBool());
	ui_debugger->setChecked(settings.value(DEBUGGER_ENABLED, false).toBool());

	settings.endGroup();
}

