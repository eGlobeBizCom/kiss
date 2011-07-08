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

#ifndef __TARGET_MANAGER_H__
#define __TARGET_MANAGER_H__

#include "TargetInterface.h"

#include <QString>
#include <QPluginLoader>
#include <QIcon>

class TargetManager
{
public:
	static TargetManager& ref();
	TargetInterface* get(const QString& targetName);
	void unloadAll();
	
	QStringList targets();
	QString displayName(const QString& target);
	QString targetPath(const QString& target);
	QStringList templateFolders(const QString& target);
	QStringList templates(const QString& target, const QString& folder = "");
	QIcon templateIcon(const QString& target, const QString& _template, const QString& folder = "");
	QStringList allSupportedExtensions();
private:
	TargetManager();
	TargetManager(const TargetManager&);
	~TargetManager();
	
	bool loadPlugin(const QString& targetName);
	void unloadPlugin(const QString& targetName);	
	
	QMap<QString, QPluginLoader*> m_plugins;
};

#endif