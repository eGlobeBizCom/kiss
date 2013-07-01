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

#ifndef _MAIN_WINDOW_HPP_
#define _MAIN_WINDOW_HPP_

#include "ui_MainWindow.h"
#include "editor_settings_dialog.hpp"
#include "theme_settings_dialog.hpp"
#include "tab.hpp"

#include "menu_manager.hpp"
#include "project_manager.hpp"
#include "project_model.hpp"
#include "main_window_menu.hpp"
#include "target.hpp"

#include <pcompiler/output.hpp>
#include <QEvent>
#include <QListWidget>

#define RECENTS "recents"

class QListWidgetItem;
class QProgressBar;

namespace Kiss
{
	namespace Target
	{
		class Responder;
		class MainResponder;
		class CommunicationManager;
	}
	
	namespace Template
	{
		class Manager;
	}
	
	namespace Widget
	{
		class SourceFile;

		/*! \class MainWindow
		 * \brief Holds tabs to display
		 *
		 * Very little is actually implemented in MainWindow. Instead, most is in the implementation of every Tab.
		 */
		class MainWindow : public QMainWindow, private Ui::MainWindow
		{
		Q_OBJECT
		public:
			MainWindow(QWidget *parent = 0);
			~MainWindow();

			void closeEvent(QCloseEvent *e);

			/*!
			 * Opens a file with SourceFile tab
			 * \param filePath Path to file
			 */
			bool openFile(const QString &filePath, const Project::ProjectPtr &project = Project::ProjectPtr());
			bool memoryOpen(const QByteArray &ba, const QString &assocPath);
			Project::ProjectPtr newProject(const QString &projectPath);
			Project::ProjectPtr openProject(const QString &projectFilePath);
			void closeProject(const Project::ProjectPtr &project);

			void initMenus();

			/*! Sets Window Title
			 * \param title Title to append to primary window name
			 */
			void setTitle(const QString &title);

			/*! Sets Tab title
			 * \param widget Tab to set
			 * \param string String to set tab's text to.
			 */
			void setTabName(QWidget *widget, const QString &string);

			/*! Sets Tab icon
			 * \param widget Tab to set
			 * \param icon icon to set
			 */
			void setTabIcon(QWidget *widget, const QIcon &icon);

			/*! Sets Window's status message
			 * \param message Message to display
			 * \param time Time to display in milliseconds
			 */
			void setStatusMessage(const QString &message, int time = 0);
		
			void setOutputList(const Compiler::OutputList &output);
			
			void hideErrors();

			/*! 
			 * Deletes tab at given index. Does NOT clean up tab via close method.
			 * \param index Tab to add
			 */
			void deleteTab(int index);

			/*! 
			 * Adds given tab window. Calls tab setup functions.
			 * \param tab Tab to add
			 */
			void addTab(Tab *tab);

			void moveToTab(Tab *tab);

			QTabWidget *tabWidget();

			QList<Tab *> tabs();

			template<typename T>
			QList<T *> tabs() {
				QList<T *> ret;
				QList<Tab *> all = tabs();
				foreach(Tab *tab, all) {
					T *t = dynamic_cast<T*>(tab);
					if(t) ret.append(t);
				}
				return ret;
			}

			/*! 
			 * Closes all but given tab 
			 * \param tab Tab to keep open
			 */
			void closeAllOthers(Tab *tab);

			/*! Reinits menus for current tab */
			void refreshMenus();

			bool eventFilter(QObject *target, QEvent *event);

			void projectOpenSettings(const Kiss::Project::ProjectPtr &project);

			Menu::Manager *menuManager();
			Menu::Menuable *menuable(const QString &name);
			QList<Menu::Menuable *> menuablesExcept(const QStringList &names);
			void deactivateMenuablesExcept(const QStringList &names);
			QList<Menu::Menuable *> menuables();
			void activateMenuable(const QString &name, QObject *on);
			QStringList standardMenus() const;
			
			Template::Manager *templateManager() const;

			bool canClose();
			bool canGoPrevious();
			bool canGoNext();
			
			Target::Responder *mainResponder() const;
			
			friend class Menu::MainWindowMenu;

		public slots:
			void importTemplatePack();
			void newTemplatePack();
			Project::ProjectPtr newProject();
			void newFile();
			void open();
			void openProject();
			void closeProject();
			void next();
			void previous();
			void closeTab(int index, bool force = false);
			void closeCurrentTab(bool force = false);
			void closeProjectTabs(const Kiss::Project::ProjectPtr &project);
			bool closeFile(const QString &file);
			void about();
			void settings();
			void theme();

			const bool download();
			const bool compile();
			const bool run();

			const bool changeTarget();
			const bool changeTarget(Kiss::Project::ProjectPtr project);

			void showCompilerOutput(const Compiler::OutputList &results);

			void showProjectDock(bool show = true);
			void hideProjectDock();
			
			void toggleCommunicationWidget();

			void projectContextMenu(const QPoint &pos);

			QList<QObject *> tabs(const QString &type);

		signals:
			void settingsUpdated();
			void updateActivatable();

		private slots:
			void on_ui_tabWidget_currentChanged(int i);
			void projectAddNew();
			void projectAddExisting();
			void projectAddExisting(QStringList files);
			void projectRenameFile();
			void projectRemoveFile();
			void deleteProject();
			void projectOpenSettings();
			void openRecent();

			void errorClicked(QListWidgetItem *item);

			void showContextMenuForError(const QPoint &pos);

			void projectClicked(const QModelIndex &index);
			void projectDoubleClicked(const QModelIndex &index);
			
			// FIXME: This will cause issues if we ever support
			// more than one main window
			void authenticateTarget(const Kiss::Target::TargetPtr &target,
				Kiss::Target::CommunicationManager *manager);
				
			void oldDeviceSoftware(const Kiss::Target::TargetPtr &target);
			void oldHostSoftware(const Kiss::Target::TargetPtr &target);

		private:
			Tab *m_currentTab;
			EditorSettingsDialog m_editorSettingsDialog;
			Dialog::ThemeSettings m_themeSettingsDialog;
			QMap<QWidget *, Tab *> m_lookup;
			Menu::Manager m_menuManager;
			Template::Manager *m_templateManager;
			Project::Manager m_projectManager;
			QList<Menu::Menuable *> m_menuables;
			
			QProgressBar *m_commProgress;
			
			Project::Model m_projectsModel;
			QMenu *m_projectContextMenu;
			QMenu *m_fileContextMenu;

			Target::Responder *m_mainResponder;

			void addLookup(Tab *tab);
			void removeLookup(QWidget *widget);
			Tab *lookup(QWidget *widget) const;

			void showErrorMessages(bool verbose = false);

			bool commPreconditions(const Kiss::Project::ProjectPtr &project);
		};
	}
}

#endif
