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

#ifndef _DEVELOPERMENU_H_
#define _DEVELOPERMENU_H_

#include "BuildOptions.h"

class DeveloperMenu;

#ifdef BUILD_DEVELOPER_TOOLS

#include "Activatable.h"
#include "MenuManager.h"
#include "SourceFile.h"
#include "ConcreteMenuable.h"
#include "MainWindow.h"

class DeveloperMenu : public ConcreteMenuable, public ActivatableObject
{
Q_OBJECT
public:
	DeveloperMenu(MainWindow* mainWindow);
	
	static QString menuName();
	
protected slots:
	virtual void triggered();
	
private:
	MainWindow* m_mainWindow;
	
	QAction* injectScript;
	QAction* declTab;
	QAction* uninstallAll;
	QAction* writeFullProject;
	QAction* writeDeltaProject;
};

#endif

#endif