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

#include "web_tab.hpp"

#ifdef BUILD_WEB_TAB

#include "main_window.hpp"
#include "web_tab_menu.hpp"
#include "main_window_menu.hpp"

#include <QUrl>
#include <QToolBar>
#include <QWebHistory>
#include <QDesktopServices>
#include <QDebug>
#include <QDir>

using namespace Kiss::Widget;

struct CurrentDirectoryMacro : Macro
{
	QString macro(const QString& with) const
	{
		return QDir::currentPath();
	}
};

WebTab::WebTab(MainWindow *parent)
	: QWidget(parent),
	Tab(this, parent)
{
	setupUi(this);
	
	connect(ui_webView, SIGNAL(titleChanged(const QString&)), this, SLOT(updateTitle(const QString&)));
	connect(ui_webView, SIGNAL(urlChanged(const QUrl&)), this, SLOT(updateUrl(const QUrl&)));
	
	ui_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
	connect(webView(), SIGNAL(linkClicked(const QUrl&)), this, SLOT(linkClicked(const QUrl&)));
	
	ui_frameFind->hide();
	
	m_fragmentMacro["KISS_CWD"] = new CurrentDirectoryMacro();
	
	ui_webView->history()->clear();
}

WebTab::~WebTab()
{
	ActivatableObject* activatable = dynamic_cast<ActivatableObject*>(mainWindow()->menuable(Menu::WebTabMenu::menuName()));
	if(activatable) activatable->deactivateIfActive(this);
}

void WebTab::activate()
{
	QList<Menu::Menuable*> menus = mainWindow()->menuablesExcept(mainWindow()->standardMenus());
	foreach(Menu::Menuable* menu, menus) {
		ActivatableObject* activatable = dynamic_cast<ActivatableObject*>(menu);
		if(activatable) activatable->setActive(0);
	}
	Menu::WebTabMenu *webTabMenu = dynamic_cast<Menu::WebTabMenu *>(mainWindow()->menuable(Menu::WebTabMenu::menuName()));
	if(webTabMenu) webTabMenu->setActive(this);

	emit updateActivatable();
}

bool WebTab::beginSetup() { return true; }

void WebTab::completeSetup()
{
	updateTitle("");
	updateUrl(QUrl::fromUserInput("about:blank"));
}

void WebTab::updateTitle(const QString& title)
{
	QString local = title;
	if(title.length() > 40) {
		local.truncate(40);
		local += "...";
	}
	mainWindow()->setTabName(this, local.length() == 0 ? tr("Untitled Browser") : local);
}

void WebTab::updateUrl(const QUrl& url)
{
	
	actionBack->setEnabled(ui_webView->history()->canGoBack());
	actionForward->setEnabled(ui_webView->history()->canGoForward());
	
	ui_urlBar->setText(url.toString());
	mainWindow()->setStatusMessage(tr("Loaded ") + url.toString(), 5000);
}

void WebTab::on_actionGo_triggered() { ui_webView->load(QUrl::fromUserInput(ui_urlBar->text())); }
void WebTab::copy() { ui_webView->triggerPageAction(QWebPage::Copy); }
void WebTab::cut() { ui_webView->triggerPageAction(QWebPage::Cut); }
void WebTab::paste() { ui_webView->triggerPageAction(QWebPage::Paste); }
void WebTab::back() { ui_webView->triggerPageAction(QWebPage::Back); }
void WebTab::forward() { ui_webView->triggerPageAction(QWebPage::Forward); }
void WebTab::go() { on_actionGo_triggered(); }
bool WebTab::close() { return true; }

void WebTab::load(QString url, bool hideUrl)
{
	mainWindow()->setTabName(this, tr("Loading..."));
	m_prevUrl = QUrl::fromUserInput(url);
	ui_webView->load(QUrl::fromUserInput(url));
	actionGo->setEnabled(!hideUrl);
	ui_urlBar->setVisible(!hideUrl);
	ui_goButton->setVisible(!hideUrl);
	ui_load->setVisible(!hideUrl);
}

QString WebTab::current()
{
	return ui_urlBar->text();
}

QWebHistory* WebTab::history()
{
	return ui_webView->history();
}

QWebView* WebTab::webView()
{
	return ui_webView;
}

void WebTab::on_ui_prevFind_clicked() { ui_webView->findText(ui_find->text(), QWebPage::FindBackward); }
void WebTab::on_ui_nextFind_clicked() { ui_webView->findText(ui_find->text()); }

void WebTab::on_ui_webView_loadFinished(bool ok)
{
	ui_load->hide();
	
	emit updateActivatable();
	
	if(ok) return;
	
	actionGo->setEnabled(false);
	ui_urlBar->setVisible(false);
	ui_goButton->setVisible(false);
	
	QFile file(":/webtab_fail/index.html");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

	QByteArray data = file.readAll();
	#ifndef Q_OS_WIN
	data.replace(KISS_BACKGROUND, palette().color(QPalette::Background).name().toAscii());
	#else
	data.replace(KISS_BACKGROUND, "#FFFFFF");
	#endif

	ui_webView->setHtml(data.data());

	actionOpenInBrowser->setEnabled(false);
}

void WebTab::openInBrowser() { QDesktopServices::openUrl(ui_webView->url()); }
void WebTab::find() { ui_frameFind->show(); }
void WebTab::refresh() { ui_webView->reload(); }

void WebTab::refreshSettings() {}

void WebTab::linkClicked(const QUrl& url)
{
	qWarning() << "Prev URL:" << m_prevUrl;
	if(url.scheme() != "kiss") {
		m_prevUrl = ui_webView->url();
		ui_webView->load(url);
	}
	
	QString auth = url.authority();
	qWarning() << "Auth" << auth;
	if(auth == "new") {
		mainWindow()->newFile();
		return;
	}
	if(auth == "open") {
		mainWindow()->open();
		return;
	}
	if(auth == "settings") {
		mainWindow()->settings();
		return;
	}
	if(auth == "openinbrowser") {
		openInBrowser();
		return;
	}
	if(auth == "openprevinbrowser") {
		QDesktopServices::openUrl(m_prevUrl);
		return;
	}
	
	QString fragment = m_fragmentMacro.apply(url.fragment());
	qWarning() << fragment;
	if(auth == "newbrowser") {
		WebTab* tab = new WebTab();
		tab->load(fragment);
		mainWindow()->addTab(tab);
		return;
	}
	if(auth == "openfile") {
		mainWindow()->openFile(fragment);
		return;
	}
	if(auth == "external") {
		QDesktopServices::openUrl(QUrl::fromUserInput(fragment));	
		return;
	}
}

#endif
