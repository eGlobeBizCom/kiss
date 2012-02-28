#include "Process.h"

#include "ScriptUtils.h"

#include <QProcessEnvironment>
#include <QDebug>

#include <QScriptEngine>

Process::Process() : m_program(""), m_process(new QProcess(this))
{
	init();
}

Process::Process(const QString& program) : m_program(program), m_process(new QProcess(this))
{
	init();
}

void Process::init()
{
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
	connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
}

const QString& Process::program() const
{
	return m_program;
}

void Process::setProgram(const QString& program)
{
	m_program = program;
}

QScriptValue Process::systemEnvironment() const
{
	if(!engine()) return QScriptValue();
	QScriptValue ret = engine()->newObject();
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	// env.keys() not introduced until 4.8
	QStringList keys;
	foreach(const QString& var, env.toStringList()) keys << var.split("=")[0];
	foreach(const QString& key, keys) ret.setProperty(key, env.value(key));
	return ret;
}

QScriptValue Process::environment() const
{
	if(!engine()) return QScriptValue();
	QScriptValue ret = engine()->newObject();
	QProcessEnvironment env = m_process->processEnvironment();
	// env.keys() not introduced until 4.8
	QStringList keys;
	foreach(const QString& var, env.toStringList()) {
		keys << var.split("=")[0];
		
	}
	qWarning() << keys;
	foreach(const QString& key, keys) ret.setProperty(key, env.value(key));
	return ret;
}

void Process::setEnvironment(const QScriptValue& obj)
{
	QProcessEnvironment env;
	QMap<QString, QString> stringMap = ScriptUtils::objectStringMap(obj);
	foreach(const QString& key, stringMap.keys()) env.insert(key, stringMap[key]);
	m_process->setProcessEnvironment(env);
}

void Process::start(const QStringList& args)
{
	m_process->start(m_program, args);
}

void Process::terminate()
{
	m_process->terminate();
}

void Process::waitForFinished(int timeout)
{
	m_process->waitForFinished(timeout);
}

int Process::exitCode() const
{
	return m_process->exitCode();
}

QString Process::stdout() const
{
	return QString(m_process->readAllStandardOutput());
}

QString Process::stderr() const
{
	return QString(m_process->readAllStandardError());
}

void Process::processFinished(int code, QProcess::ExitStatus status)
{
	emit finished(code, status == QProcess::NormalExit);
}

void Process::processError(QProcess::ProcessError err)
{
	QString message("Unknown Error");
	switch(err) {
		case QProcess::FailedToStart: message = "Failed to Start"; break;
		case QProcess::Crashed: message = "Crashed"; break;
		case QProcess::Timedout: message = "Timed out"; break;
		case QProcess::WriteError: message = "Write Error"; break;
		case QProcess::ReadError: message = "Read Error"; break;
	}
	
	emit error(message);
}