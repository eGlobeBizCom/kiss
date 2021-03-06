#include "communication_progress_bar.hpp"
#include "communication_manager.hpp"

using namespace kiss;
using namespace kiss::widget;

CommunicationProgressBar::CommunicationProgressBar(target::CommunicationManager *manager, QWidget *parent)
	: QProgressBar(parent),
	m_manager(manager),
	m_done(0),
	m_total(0)
{
	setMinimum(0);
	
	setFormat(tr("%v of %m"));
	
	setMaximumSize(QSize(100, 100));
	
	connect(m_manager, SIGNAL(admitted(kiss::target::CommunicationEntryPtr)), SLOT(admitted()));
	connect(m_manager, SIGNAL(finished(kiss::target::CommunicationEntryPtr, kiss::target::Target::ReturnCode)), SLOT(finished()));
	connect(m_manager, SIGNAL(queueFinished()), SLOT(queueFinished()));
}

target::CommunicationManager *CommunicationProgressBar::manager() const
{
	return m_manager;
}

void CommunicationProgressBar::paintEvent(QPaintEvent *event)
{
	if(m_total == 0) return;
	QProgressBar::paintEvent(event);
}

void CommunicationProgressBar::mousePressEvent(QMouseEvent *event)
{
	emit clicked();
}

void CommunicationProgressBar::admitted()
{
	setMaximum(++m_total);
}

void CommunicationProgressBar::finished()
{
	setValue(++m_done);
}

void CommunicationProgressBar::queueFinished()
{
	m_done = m_total = 0;
	update();
}