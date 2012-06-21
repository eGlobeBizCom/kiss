/**************************************************************************
 *  Copyright 2007-2011 KISS Institute for Practical Robotics             *
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

#ifndef __TEMPLATEDIALOG_H__
#define __TEMPLATEDIALOG_H__

#include "ui_TemplateDialog.h"
#include <QDialog>
#include <QFile>

class TemplateDialog : public QDialog, private Ui::TemplateDialog
{
	Q_OBJECT
public:
	TemplateDialog(QWidget* parent = 0);
	
	int exec();
	
	// Returns to the path of the target file for the selected target
	
	QString selectedTypeName() const;
	QString templateFile();
	
private slots:
	void on_ui_types_currentItemChanged(QListWidgetItem* current, QListWidgetItem* prev);
	void on_ui_templates_itemDoubleClicked(QTreeWidgetItem* current);
	
	void on_ui_templates_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*);
	void on_ui_remove_clicked();
	
private:
	void addTemplates(const QString& target, QTreeWidgetItem* parentItem, const QString& parent);
	void addUserTemplates(const QString& target);
};

#endif
