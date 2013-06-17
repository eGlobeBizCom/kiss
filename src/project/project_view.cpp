#include "project_view.hpp"

#include <QUrl>
#include <QDebug>

using namespace Kiss::Project;

ProjectView::ProjectView(QWidget* parent)
: QTreeView(parent)
{
	setAcceptDrops(true);
	setDropIndicatorShown(true);
}

void ProjectView::dragEnterEvent(QDragEnterEvent *event)
{
	activateWindow();
	raise();
	event->accept();
}

void ProjectView::dragMoveEvent(QDragMoveEvent *event)
{
	const QModelIndex& index = indexAt(event->pos());
	if(index.isValid()) {
		if(m_model->isProjectRoot(index)) setCurrentIndex(index);
		// TODO: This assumes a strictly two-level project model
		else setCurrentIndex(index.parent());
	}
	event->setDropAction(Qt::MoveAction);
	event->accept();
}

void ProjectView::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	QStringList files;
	foreach(QUrl url, urls)
	if(url.isLocalFile()) files << url.toLocalFile();

	emit filesDropped(files);
}

void ProjectView::setModel(Model* model)
{
	m_model = model;
	QTreeView::setModel(model);
}