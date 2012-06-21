#include "Documentation.h"

#ifdef BUILD_DOCUMENTATION_TAB

#include "MainWindow.h"
#include "ResourceHelper.h"
#include "TitleDescriptionWidget.h"
#include "WebTab.h"
#include "DocumentationManager.h"

#include <QListWidget>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>

#include "MainWindowMenu.h"

Documentation::Documentation(MainWindow* parent) : TabbedWidget(new QListWidget(parent), parent)
{
	QListWidget* list = qobject_cast<QListWidget*>(widget());
	//list->setAlternatingRowColors(true);
}

void Documentation::activate()
{
	QList<Menuable*> menus = mainWindow()->menuablesExcept(mainWindow()->standardMenus());
	foreach(Menuable* menu, menus) {
		ActivatableObject* activatable = dynamic_cast<ActivatableObject*>(menu);
		if(activatable) activatable->setActive(0);
	}
}

bool Documentation::beginSetup()
{
	QListWidget* list = qobject_cast<QListWidget*>(widget());
	const QString providedBy("Provided by ");
	foreach(const DocumentationLocation& location, DocumentationManager::ref().locations()) {
		QListWidgetItem* item = new QListWidgetItem(list);
		item->setIcon(ResourceHelper::ref().icon("text-x-generic"));
		item->setData(Qt::UserRole, location.location());
		item->setSizeHint(QSize(100, 65));
		list->setItemWidget(item,
			new TitleDescriptionWidget(
				location.name(),
				location.description(),
				location.decoration(),
				list)
			);
	}
	list->setIconSize(QSize(50, 50));
	connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	
	return true;
}

void Documentation::completeSetup()
{
	mainWindow()->setTabName(widget(), "Documentation");
}

bool Documentation::close()
{
	return true;
}

void Documentation::refreshSettings()
{
	
}

void Documentation::itemDoubleClicked(QListWidgetItem* item)
{
	const QString& location = item->data(Qt::UserRole).toString();
	if(location.endsWith("pdf")) {
		QDesktopServices::openUrl(QUrl::fromUserInput(location));
		return;
	}
	
#ifdef Q_OS_WIN
	const QString& sysLocation = "file:///" + location;
#else
	const QString& sysLocation = "file://" + location;
#endif
	
	UiEventManager::ref().sendEvent(UI_EVENT_OPEN_MANUAL);

#ifdef BUILD_WEB_TAB
	QList<WebTab*> webTabs = mainWindow()->tabs<WebTab>();
	foreach(WebTab* tab, webTabs) {
		if(tab->current() == sysLocation) {
			mainWindow()->moveToTab(tab);
			return;
		}
	}
	
	WebTab* tab = new WebTab(mainWindow());
	mainWindow()->addTab(tab);
	
	tab->load(sysLocation, true);
#else
	QDesktopServices::openUrl(QUrl::fromUserInput(sysLocation));
#endif
}

#endif

