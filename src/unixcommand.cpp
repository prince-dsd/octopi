/*
* This file is part of Octopi, an open-source GUI for ArchLinux pacman.
* Copyright (C) 2013  Alexandre Albuquerque Arnt
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#include "unixcommand.h"
#include "strconstants.h"
#include "wmhelper.h"
#include <iostream>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QTextStream>
#include <QtNetwork/QNetworkInterface>

QFile *UnixCommand::m_temporaryFile = 0;

/*
 * Executes given command and returns the StandardError Output.
 */
QString UnixCommand::runCommand(const QString& commandToRun)
{
  QProcess proc;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  proc.setProcessEnvironment(env);
#endif

  proc.start(commandToRun);
  proc.waitForStarted();
  proc.waitForFinished(-1);
  QString res = proc.readAllStandardError();
  proc.close();
  return res;
}

/*
 * Executes the CURL command and returns the StandardError Output, if result code <> 0.
 */
QString UnixCommand::runCurlCommand(const QString& commandToRun){
  QProcess proc;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  proc.setProcessEnvironment(env);
#endif

  proc.start(commandToRun);
  proc.waitForStarted();
  proc.waitForFinished(-1);

  QString res("");
  if (proc.exitCode() != 0)
    res = proc.readAllStandardError();

  proc.close();
  return res;
}

/*
 * Returns the path of given executable
 */
QString UnixCommand::discoverBinaryPath(const QString& binary){
  QProcess *proc = new QProcess;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  proc->setProcessEnvironment(env);
#endif

  proc->start("/bin/sh -c \"which " + binary + "\"");
  proc->waitForFinished();
  QString res = proc->readAllStandardOutput();

  proc->close();
  delete proc;
  res = res.remove('\n');

  //If it still didn't find it, try "/sbin" dir...
  if (res.isEmpty()){
    QFile fbin("/sbin/" + binary);
    if (fbin.exists()){
      res = "/sbin/" + binary;
    }
  }

  return res;
}

/*
 * Cleans Pacman's package cache
 */
void UnixCommand::cleanPacmanCache()
{
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  QFile *ftemp = getTemporaryFile();
  QTextStream out(ftemp);
  out << "pacman -Sc --noconfirm";
  out.flush();
  ftemp->close();

  QString command = WMHelper::getSUCommand() + " " + ftemp->fileName();
  pacman.start(command);
  pacman.waitForStarted();

  //pacman.write("y"); //Yes, we want to remove ALL files from cache
  //pacman.write("n"); //No, we do not want to remove unused repositories
  //pacman.closeWriteChannel();

  pacman.waitForFinished();
}

/*
 * Returns a string containing all packages no one depends on
 */
QByteArray UnixCommand::getUnrequiredPackageList()
{
  QByteArray result("");
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  args << "-Qt";
  pacman.start("pacman", args);

  pacman.waitForFinished(-1);
  result = pacman.readAllStandardOutput();

  return result;
}

/*
 * Returns a string containing all packages that are outdated since last DB sync
 */
QByteArray UnixCommand::getOutdatedPackageList()
{
  QByteArray result("");
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  args << "-Qu";
  pacman.start("pacman", args);

  pacman.waitForFinished(-1);
  result = pacman.readAllStandardOutput();

  return result;
}

/*
 * Returns a string containing all packages that are not contained in any repository
 * (probably the ones installed by a tool such as yaourt)
 */
QByteArray UnixCommand::getForeignPackageList()
{
  QByteArray result("");
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  args << "-Qm";
  pacman.start("pacman", args);

  pacman.waitForFinished(-1);
  result = pacman.readAllStandardOutput();

  return result;
}

/*
 * Returns a string with the list of all packages available in all repositories
 * (installed + not installed)
 */
QByteArray UnixCommand::getPackageList()
{
  QByteArray result("");
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  args << "-Sl";
  pacman.start("pacman", args);

  pacman.waitForFinished(-1);
  result = pacman.readAllStandardOutput();

  return result;
}

/*
 * Given a package name and if it is default to the official repositories,
 * returns a string containing all the fields of its information
 * (ex: name, description, version, dependsOn...)
 */
QByteArray UnixCommand::getPackageInformation(const QString &pkgName, bool foreignPackage = false)
{
  QByteArray result("");
  QProcess pacman;
  QStringList args;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  if(foreignPackage)
  {
    args << "-Qi";
  }
  else
  {
    args << "-Si";
  }

  args << pkgName;
  pacman.start("pacman", args);

  pacman.waitForFinished(-1);
  result = pacman.readAllStandardOutput();

  pacman.close();

  return result;
}

/*
 * Given a package name, returns a string containing all the files inside it
 */
QByteArray UnixCommand::getPackageContents(const QString& pkgName)
{
  QByteArray res;
  QProcess pacman;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  QStringList args;
  args << "-Ql";
  args << pkgName;

  pacman.start ( "pacman", args );
  pacman.waitForFinished(-1);
  res = pacman.readAllStandardOutput();
  pacman.close();

  return res;
}

/*
 * Retrives the list of package groups
 */
QByteArray UnixCommand::getPackageGroups()
{
  QByteArray res;
  QProcess pacman;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  QStringList args;
  args << "-Sg";

  pacman.start ( "pacman", args );
  pacman.waitForFinished(-1);
  res = pacman.readAllStandardOutput();
  pacman.close();

  return res;
}

/*
 * Given a group name, returns a string containing all packages from it
 */
QByteArray UnixCommand::getPackagesFromGroup(const QString &groupName)
{
  QByteArray res;
  QProcess pacman;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  QStringList args;
  args << "-Sg";
  args << groupName;

  pacman.start ( "pacman", args );
  pacman.waitForFinished(-1);
  res = pacman.readAllStandardOutput();
  pacman.close();

  return res;
}

/*
 * Retrieves the list of targets needed to update the entire system or a given package
 */
QByteArray UnixCommand::getTargetUpgradeList(const QString &pkgName)
{
  QByteArray res;
  QProcess pacman;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  QString args;

  if(!pkgName.isEmpty())
  {
    args += "-Sp " + pkgName;
  }
  else
  {
    args += "-Spu"; //this is the complete system upgrade!
  }

  pacman.start ( "pacman " + args );
  pacman.waitForFinished(-1);
  res = pacman.readAllStandardOutput();
  pacman.close();

  return res;
}

/*
 * Given a package name, retrieves the list of all targets needed for its removal
 */
QByteArray UnixCommand::getTargetRemovalList(const QString &pkgName)
{
  QByteArray res;
  QProcess pacman;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  pacman.setProcessEnvironment(env);
#endif

  pacman.start ( "pacman -Rpcs " + pkgName );
  pacman.waitForFinished(-1);
  res = pacman.readAllStandardOutput();
  pacman.close();

  return res;
}

/*
 * Retrieves the system arch
 */
QString UnixCommand::getSystemArchitecture()
{
  QStringList slParam;
  QProcess proc;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  proc.setProcessEnvironment(env);
#endif

  slParam << "-m";
  proc.start("uname", slParam);
  proc.waitForFinished();

  QString out = proc.readAllStandardOutput();
  proc.close();

  return out;
}

/*
 * Checks if we have internet access!
 */
bool UnixCommand::hasInternetConnection()
{
  QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
  bool result = false;

  for (int i = 0; i < ifaces.count(); i++){
    QNetworkInterface iface = ifaces.at(i);

    if ( iface.flags().testFlag(QNetworkInterface::IsUp)
         && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) ){
      for (int j=0; j<iface.addressEntries().count(); j++){
        /*
         We have an interface that is up, and has an ip address
         therefore the link is present.

         We will only enable this check on first positive,
         all later results are incorrect
        */
        if (result == false)
          result = true;
      }
    }
  }
  return result;
}

/*
 * We must check if KTSUSS version is 1.3 or 1.4.
 */
bool UnixCommand::isKtsussVersionOK()
{
  QProcess proc;

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  proc.setProcessEnvironment(env);
#endif

  QStringList slParam("-v");
  proc.start("ktsuss", slParam );
  proc.waitForFinished();

  QString out = proc.readAllStandardOutput();
  proc.close();

  if (out.indexOf("ktsuss 1.3") != -1 || out.indexOf("ktsuss 1.4") != -1)
    return true;
  else
    return false;
}

/*
 * Checks if the given executable is available somewhere in the system
 */
bool UnixCommand::hasTheExecutable( const QString& exeName )
{
  QProcess* proc = new QProcess();
  proc->setProcessChannelMode(QProcess::MergedChannels);
  QString sParam = "\"which " + exeName + "\"";
  proc->start("/bin/sh -c " + sParam);
  proc->waitForFinished();

  QString out = proc->readAllStandardOutput();
  proc->close();

  delete proc;
  if (out.count("which") > 0) return false;
  else return true;
}

/*
 * Does some garbage collection, removing uneeded files
 */
void UnixCommand::removeTemporaryFiles()
{
  QDir tempDir(QDir::tempPath());
  QStringList nameFilters;
  nameFilters << "qtsingleapp*" << "gpg*" << ".qt_temp_*";
  QFileInfoList list = tempDir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden);

  foreach(QFileInfo file, list){
    QFile fileAux(file.filePath());
    //std::cout << "Found: " << file.fileName().toAscii().data() << std::endl;

    if (!file.isDir()){
      //std::cout << "trying to remove " << file.fileName().toAscii().data() << std::endl;
      fileAux.remove();
    }
    else{
      QDir dir(file.filePath());
      QFileInfoList listd = dir.entryInfoList(QDir::Files | QDir::System);

      foreach(QFileInfo filed, listd){
        QFile fileAuxd(filed.filePath());
        fileAuxd.remove();
      }

      dir.rmdir(file.filePath());
    }
  }
}

/*
 * Given a filename, checks if it is a text file
 */
bool UnixCommand::isTextFile(const QString& fileName)
{
  QProcess *p = new QProcess();

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  p->setProcessEnvironment(env);
#endif

  QStringList s(fileName);
  p->start( "file", s );
  p->waitForFinished();

  QByteArray output = p->readAllStandardOutput();
  p->close();
  delete p;

  int from = output.indexOf(":", 0)+1;

  return (((output.indexOf( "ASCII", from ) != -1) ||
          (output.indexOf( "text", from ) != -1) ||
          (output.indexOf( "empty", from ) != -1)) &&
          (output.indexOf( "executable", from) == -1));
}

/*
 * Executes the given commandlist using QProcess async technology
 */
void UnixCommand::executePackageActions( const QStringList& commandList )
{
  QFile *ftemp = getTemporaryFile();
  QTextStream out(ftemp);

  foreach(QString line, commandList)
    out << line;

  out.flush();
  ftemp->close(); 

  QString command = WMHelper::getSUCommand() + " " + ftemp->fileName();

  m_process->start(command);
}

/*
 * Puts all Standard output of the member process into a member string
 */
void UnixCommand::processReadyReadStandardOutput()
{
  m_readAllStandardOutput = m_process->readAllStandardOutput();
}

/*
 * Puts all StandardError output of the member process into a member string
 */
void UnixCommand::processReadyReadStandardError()
{
  m_readAllStandardError = m_process->readAllStandardError();
  m_errorString = m_process->errorString();
}

/*
 * Retrieves Standard output of member process
 */
QString UnixCommand::readAllStandardOutput()
{
  return m_readAllStandardOutput;
}

/*
 * Retrieves StandardError output of member process
 */
QString UnixCommand::readAllStandardError()
{
  return m_readAllStandardError;
}

/*
 * Retrieves ErrorString of member process
 */
QString UnixCommand::errorString()
{
  return m_errorString;
}

/*
 * UnixCommand's constructor: the relevant environment english setting and the connectors
 */
UnixCommand::UnixCommand(QObject *parent): QObject()
{
  m_process = new QProcess(parent);

#if QT_VERSION >= 0x040600
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "us_EN");
  m_process->setProcessEnvironment(env);
#endif

  QObject::connect(m_process, SIGNAL( started() ), this,
                   SIGNAL( started() ));
  QObject::connect(this, SIGNAL( started() ), this,
                   SLOT( processReadyReadStandardOutput() ));

  QObject::connect(m_process, SIGNAL( readyReadStandardOutput() ), this,
                   SIGNAL( readyReadStandardOutput() ));
  QObject::connect(this, SIGNAL( readyReadStandardOutput() ), this,
                   SLOT( processReadyReadStandardOutput() ));

  QObject::connect(m_process, SIGNAL( finished ( int, QProcess::ExitStatus )), this,
                   SIGNAL( finished ( int, QProcess::ExitStatus )) );
  QObject::connect(this, SIGNAL( finished ( int, QProcess::ExitStatus )), this,
                   SLOT( processReadyReadStandardOutput() ));

  QObject::connect(m_process, SIGNAL( readyReadStandardError() ), this,
                   SIGNAL( readyReadStandardError() ));
  QObject::connect(this, SIGNAL( readyReadStandardError() ), this,
                   SLOT( processReadyReadStandardError() ));
}
