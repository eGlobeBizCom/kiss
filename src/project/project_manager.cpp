#include "project_manager.hpp"
#include "project.hpp"

#include <QDebug>

using namespace Kiss::Project;

Manager::Manager()
{
	
}

Manager::~Manager()
{
	m_projects.clear();
}

void Manager::openProject(const ProjectPtr &project)
{
	if(m_projects.contains(project)) return;
	
	m_projects.append(project);
	emit projectOpened(project);
}

void Manager::closeProject(const ProjectPtr &project)
{
	if(!m_projects.contains(project)) return;
	ProjectPtr ref = project;
	m_projects.removeAll(project);
	emit projectClosed(ref);
}

const ProjectPtrList &Manager::projects() const
{
	return m_projects;
}

const QStringList Manager::hiddenExtensions()
{
	return QStringList() << PROJECT_EXT << LINKS_EXT;
}