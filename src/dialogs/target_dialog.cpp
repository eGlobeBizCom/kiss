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

#include "target_dialog.hpp"

#include "interface.hpp"
#include "interface_manager.hpp"

#include "manual_target_dialog.hpp"

#include <QTimer>
#include <QDebug>

using namespace kiss::dialog;

Target::Target(kiss::target::InterfaceManager *manager, QWidget *parent)
	: QDialog(parent),
	m_manager(manager),
	m_model(m_manager),
	m_interfaceModel(m_manager)
{
	setupUi(this);
	ui_targets->setModel(&m_model);
	m_interfaceModel.setAllInterface(true);
	ui_interfaces->setModel(&m_interfaceModel);
	
  ui_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
	ui_targetInfo->hide();
  ui_progress->hide();
	
	connect(ui_targets->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		SLOT(currentTargetChanged(const QModelIndex&)));
	connect(ui_targets,
		SIGNAL(doubleClicked(const QModelIndex&)),
		SLOT(targetChosen(const QModelIndex&)));
  connect(ui_interfaces, SIGNAL(currentIndexChanged(int)), SLOT(on_ui_refresh_clicked()));
  
  on_ui_refresh_clicked();
}

Target::~Target()
{
	foreach(kiss::target::Interface *interface, m_manager->interfaces())
		interface->invalidateResponder();
}

kiss::target::TargetPtr Target::target() const
{
	if(!m_manualTarget.isNull()) return m_manualTarget;	
	return m_model.indexToTarget(ui_targets->selectionModel()->currentIndex());
}

void Target::currentTargetChanged(const QModelIndex &index)
{
	kiss::target::TargetPtr target = m_model.indexToTarget(index);
	const QString unknown = "Unknown";
	ui_deviceName->setText(target.data() ? target->displayName() : unknown);
	ui_deviceType->setText(target.data() ? target->type() : unknown);
	ui_commPort->setText(target.data() ? target->commPort() : unknown);
	ui_serialNumber->setText(target.data() ? target->serial() : unknown);
	ui_interface->setText(target.data() ? target->interface()->name() : unknown);
	ui_version->setText(target.data() ? target->version() : unknown);
	ui_buttons->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void Target::targetChosen(const QModelIndex &index)
{
	accept();
}

void Target::on_ui_manual_clicked()
{
	ManualTarget dialog;
	if(dialog.exec() == QDialog::Rejected) {
		m_manualTarget = kiss::target::TargetPtr();
		return;
	}
	m_manualTarget = dialog.target();
	accept();
}

void Target::on_ui_refresh_clicked()
{
  ui_refresh->setEnabled(false);
  kiss::target::Interface *filter = m_interfaceModel.rowToInterface(ui_interfaces->currentIndex());
  m_model.clear();
  
  QList<kiss::target::Interface *> interfaces;
  if(!filter) interfaces = m_manager->interfaces();
  else foreach(kiss::target::Interface *interface, m_manager->interfaces())
    if(interface->name() == filter->name()) interfaces.append(interface);
  
  ui_progress->setRange(0, interfaces.size() - 1);
  ui_progress->reset();
  ui_progress->show();
    
	foreach(kiss::target::Interface *interface, interfaces) {
    connect(interface, SIGNAL(scanFinished(kiss::target::Interface *)),
      this, SLOT(interfaceFinished(kiss::target::Interface *)));
		interface->scan(&m_model);
  }
}

void Target::interfaceFinished(kiss::target::Interface *interface)
{
  disconnect(interface, SIGNAL(scanFinished(kiss::target::Interface *)),
    this, SLOT(interfaceFinished(kiss::target::Interface *)));
  int newValue = ui_progress->value() + 1;
  ui_progress->setValue(newValue);
  if(newValue != ui_progress->maximum()) return;
  
  ui_progress->hide();
  ui_refresh->setEnabled(true);
  
}