/*
* This file is part of Octopi, an open-source GUI for pacman.
* Copyright (C) 2014 Alexandre Albuquerque Arnt
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

#include "terminal.h"
#include "wmhelper.h"
#include "unixcommand.h"
#include "strconstants.h"

#include <QApplication>
#include <QProcess>
#include <QTextStream>
#include <iostream>

/*
 * This class abstracts all the relevant terminal related code
 */

Terminal::Terminal(QObject *parent, const QString &selectedTerminal) : QObject(parent)
{
  m_process = new QProcess(parent);
  m_process->setInputChannelMode(QProcess::ForwardedInputChannel);
  m_process->setProcessChannelMode(QProcess::ForwardedChannels);

  //Make the needed signal propagations...
  connect(m_process, SIGNAL(started()), this, SIGNAL(started()));
  connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SIGNAL(finished(int,QProcess::ExitStatus)));

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "C");
  env.insert("LC_MESSAGES", "C");
  m_process->setProcessEnvironment(env);
  m_selectedTerminal = selectedTerminal;
}

Terminal::~Terminal()
{
  m_process->close();
  delete m_process;
}

/*
 * Opens a terminal in the given directory
 */
/*void Terminal::openTerminal(const QString &dirName)
{
  QStringList s;

  if (m_selectedTerminal == ctn_AUTOMATIC)
  {
    if(WMHelper::isXFCERunning() && UnixCommand::hasTheExecutable(ctn_XFCE_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_XFCE_TERMINAL, s );
    }
    else if (WMHelper::isKDERunning() && UnixCommand::hasTheExecutable(ctn_KDE_TERMINAL)){
      s << "--workdir";
      s << dirName;

      if (UnixCommand::isRootRunning())
      {
        m_process->startDetached( "dbus-launch " + ctn_KDE_TERMINAL + " --workdir " + dirName);
      }
      else
      {
        m_process->startDetached( ctn_KDE_TERMINAL, s );
      }
    }
    else if (WMHelper::isTDERunning() && UnixCommand::hasTheExecutable(ctn_TDE_TERMINAL)){
      s << "--workdir";
      s << dirName;
      m_process->startDetached( ctn_TDE_TERMINAL, s );
    }
    else if (WMHelper::isLXDERunning() && UnixCommand::hasTheExecutable(ctn_LXDE_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_LXDE_TERMINAL, s );
    }
    else if (WMHelper::isMATERunning() && UnixCommand::hasTheExecutable(ctn_MATE_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_MATE_TERMINAL, s );
    }
    else if (WMHelper::isCinnamonRunning() && UnixCommand::hasTheExecutable(ctn_CINNAMON_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_CINNAMON_TERMINAL, s );
    }
    else if (WMHelper::isLXQTRunning() && UnixCommand::hasTheExecutable(ctn_LXQT_TERMINAL)){
      s << "--workdir"  ;
      s << dirName;
      m_process->startDetached( ctn_LXQT_TERMINAL, s );
    }
    else if (UnixCommand::hasTheExecutable(ctn_PEK_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_PEK_TERMINAL, s );
    }
    else if (UnixCommand::hasTheExecutable(ctn_XFCE_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_XFCE_TERMINAL, s );
    }
    else if (UnixCommand::hasTheExecutable(ctn_LXDE_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_LXDE_TERMINAL, s );
    }
    else if (UnixCommand::hasTheExecutable(ctn_RXVT_TERMINAL)){
      QString cmd;

      if (UnixCommand::isAppRunning("urxvtd"))
        cmd = "urxvtc -name Urxvt -title Urxvt -cd " + dirName;
      else
        cmd = ctn_RXVT_TERMINAL + " -name Urxvt -title Urxvt -cd " + dirName;

      m_process->startDetached( cmd );
    }
    else if (UnixCommand::hasTheExecutable(ctn_GNOME_TERMINAL)){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_GNOME_TERMINAL, s );
    }
    else if (UnixCommand::hasTheExecutable(ctn_XTERM)){
      QString cmd = ctn_XTERM +
          " -fn \"*-fixed-*-*-*-18-*\" -fg White -bg Black -title xterm -e \"" +
          "cd " + dirName + " && " + UnixCommand::getShell() + "\"";
      m_process->startDetached( cmd );
    }
  }
  else  //User has chosen a different terminal...
  {
    if(m_selectedTerminal == ctn_XFCE_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_XFCE_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_KDE_TERMINAL){
      s << "--workdir";
      s << dirName;

      if (UnixCommand::isRootRunning())
      {
        m_process->startDetached( "dbus-launch " + ctn_KDE_TERMINAL + " --workdir " + dirName);
      }
      else
      {
        m_process->startDetached( ctn_KDE_TERMINAL, s );
      }
    }
    else if (m_selectedTerminal == ctn_TDE_TERMINAL){
      s << "--workdir";
      s << dirName;
      m_process->startDetached( ctn_TDE_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_LXDE_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_LXDE_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_MATE_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_MATE_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_CINNAMON_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_CINNAMON_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_LXQT_TERMINAL){
      s << "--workdir";
      s << dirName;
      m_process->startDetached( ctn_LXQT_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_PEK_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_PEK_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_RXVT_TERMINAL){
      QString cmd;

      if (UnixCommand::isAppRunning("urxvtd"))
        cmd = "urxvtc -name Urxvt -title Urxvt -cd " + dirName;
      else
        cmd = ctn_RXVT_TERMINAL + " -name Urxvt -title Urxvt -cd " + dirName;

      m_process->startDetached( cmd );
    }
    else if (m_selectedTerminal == ctn_GNOME_TERMINAL){
      s << "--working-directory=" + dirName;
      m_process->startDetached( ctn_GNOME_TERMINAL, s );
    }
    else if (m_selectedTerminal == ctn_XTERM){
      QString cmd = ctn_XTERM +
          " -fn \"*-fixed-*-*-*-18-*\" -fg White -bg Black -title xterm -e \"" +
          "cd " + dirName + " && " + UnixCommand::getShell() + "\"";
      m_process->startDetached( cmd );
    }
  }
}*/

/*
 * Opens a terminal with root credentials
 */
/*void Terminal::openRootTerminal()
{
  if (m_selectedTerminal == ctn_AUTOMATIC || m_selectedTerminal == ctn_QTERMWIDGET)
  {
    if (UnixCommand::getLinuxDistro() == ectn_ANTERGOS && UnixCommand::hasTheExecutable(ctn_XTERM)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XTERM +
          " -fn \"*-fixed-*-*-*-18-*\" -fg White -bg Black -title xterm \"";
      m_process->startDetached(cmd);
    }
    else if(WMHelper::isXFCERunning() && UnixCommand::hasTheExecutable(ctn_XFCE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XFCE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (WMHelper::isKDERunning() && UnixCommand::hasTheExecutable(ctn_KDE_TERMINAL))
    {
      QString cmd;

      if(UnixCommand::isRootRunning())
      {
        cmd = "dbus-launch " + ctn_KDE_TERMINAL;
      }
      else
      {
        cmd = WMHelper::getSUCommand() + " \"" + ctn_KDE_TERMINAL + "\"";
      }

      m_process->startDetached(cmd);
    }
    else if (WMHelper::isTDERunning() && UnixCommand::hasTheExecutable(ctn_TDE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_TDE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (WMHelper::isLXDERunning() && UnixCommand::hasTheExecutable(ctn_LXDE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_LXDE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (WMHelper::isLXQTRunning() && UnixCommand::hasTheExecutable(ctn_LXQT_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_LXQT_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (WMHelper::isMATERunning() && UnixCommand::hasTheExecutable(ctn_MATE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_MATE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (WMHelper::isCinnamonRunning() && UnixCommand::hasTheExecutable(ctn_CINNAMON_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_CINNAMON_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (UnixCommand::hasTheExecutable(ctn_RXVT_TERMINAL))
    {
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_RXVT_TERMINAL +
          " -name Urxvt -title Urxvt \"";

      m_process->startDetached(cmd);
    }
    else if (UnixCommand::hasTheExecutable(ctn_PEK_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_PEK_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (UnixCommand::hasTheExecutable(ctn_XFCE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XFCE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (UnixCommand::hasTheExecutable(ctn_LXDE_TERMINAL)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_LXDE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (UnixCommand::hasTheExecutable(ctn_XTERM)){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XTERM +
          " -fn \"*-fixed-*-*-*-18-*\" -fg White -bg Black -title xterm \"";
      m_process->startDetached(cmd);
    }
  }
  else //User has chosen his own terminal
  {
    if (m_selectedTerminal == ctn_RXVT_TERMINAL)
    {
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_RXVT_TERMINAL +
          " -name Urxvt -title Urxvt \"";

      m_process->startDetached(cmd);
    }
    else if(m_selectedTerminal == ctn_XFCE_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XFCE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_KDE_TERMINAL)
    {
      QString cmd;

      if(UnixCommand::isRootRunning())
      {
        cmd = "dbus-launch " + ctn_KDE_TERMINAL;
      }
      else
      {
        cmd = WMHelper::getSUCommand() + " \"" + ctn_KDE_TERMINAL + "\"";
      }

      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_PEK_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_PEK_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_TDE_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_TDE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_LXDE_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_LXDE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_LXQT_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_LXQT_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_MATE_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_MATE_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_CINNAMON_TERMINAL){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_CINNAMON_TERMINAL + "\"";
      m_process->startDetached(cmd);
    }
    else if (m_selectedTerminal == ctn_XTERM){
      QString cmd = WMHelper::getSUCommand() + " \"" + ctn_XTERM +
          " -fn \"*-fixed-*-*-*-18-*\" -fg White -bg Black -title xterm \"";
      m_process->startDetached(cmd);
    }
  }
}*/

/*
 * Executes the given command list with root credentials
 */
void Terminal::runCommandInTerminal(const QStringList &commandList)
{
  QFile *ftemp = UnixCommand::getTemporaryFile();
  QTextStream out(ftemp);
  bool removedLines = false;

  foreach(QString line, commandList)
  {
    if ((line.contains("echo -e") || line.contains("read -n 1")) && m_selectedTerminal == ctn_QTERMWIDGET)
    {
      removedLines = true;
      continue;
    }

    out << line;
  }

  if (removedLines) out << "echo \"" << StrConstants::getPressAnyKey() + "\"";

  out.flush();
  ftemp->close();

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.remove("LANG");
  env.remove("LC_MESSAGES");
  env.insert("LANG", QLocale::system().name() + ".UTF-8");
  env.insert("LC_MESSAGES", QLocale::system().name() + ".UTF-8");
  m_process->setProcessEnvironment(env);
  m_process->close();

  QString cmd = UnixCommand::getShell() + " -c \"" + ftemp->fileName() + "\"";
  emit commandToExecInQTermWidget(cmd);
}

/*
 * Executes the given command list with root credentials using "octopi-helper -t"
 */
void Terminal::runOctopiHelperInTerminal(const QStringList &commandList)
{
  QFile *ftemp = UnixCommand::getTemporaryFile();
  QTextStream out(ftemp);
  bool removedLines = false;

  foreach(QString line, commandList)
  {
    if ((line.contains("echo -e") || line.contains("read -n 1")) && m_selectedTerminal == ctn_QTERMWIDGET)
    {
      removedLines = true;
      continue;
    }

    out << line << "\n";
  }

  if (removedLines) out << "echo \"" << StrConstants::getPressAnyKey() + "\"";

  out.flush();
  ftemp->close();

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.remove("LANG");
  env.remove("LC_MESSAGES");
  env.insert("LANG", QLocale::system().name() + ".UTF-8");
  env.insert("LC_MESSAGES", QLocale::system().name() + ".UTF-8");
  m_process->setProcessEnvironment(env);

  QString suCommand = WMHelper::getSUCommand();
  QString commandToRun = ctn_OCTOPI_HELPER + " -t";
  m_process->close();
  QString cmd = "sudo " + commandToRun;
  emit commandToExecInQTermWidget(cmd);
}

/*
 * Executes the given command list as normal user
 */
void Terminal::runCommandInTerminalAsNormalUser(const QStringList &commandList)
{
  QFile *ftemp = UnixCommand::getTemporaryFile();
  QTextStream out(ftemp);
  bool removedLines = false;

  foreach(QString line, commandList)
  {
    if ((line.contains("echo -e") || line.contains("read -n 1")) && m_selectedTerminal == ctn_QTERMWIDGET)
    {
      removedLines = true;
      continue;
    }

    //We must remove the "ccr/" prefix in Chakra, cos this will not work
    if(line.contains("ccr/"))
    {
      line = line.replace("ccr/", "");
    }

    out << line;
  }

  if (removedLines) out << "echo \"" << StrConstants::getPressAnyKey() + "\"";

  out.flush();
  ftemp->close();

  QString cmd;
  m_process->close();
  cmd = UnixCommand::getShell() + " -c \"" + ftemp->fileName() + "\"";
  emit commandToExecInQTermWidget(cmd);
}

/*
 * Retrives the list of available terminals in this system
 */
/*QStringList Terminal::getListOfAvailableTerminals()
{
  QStringList res;
  res.append(ctn_AUTOMATIC);

  if (UnixCommand::hasTheExecutable(ctn_PEK_TERMINAL))
    res.append(ctn_PEK_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_XFCE_TERMINAL))
    res.append(ctn_XFCE_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_LXDE_TERMINAL))
    res.append(ctn_LXDE_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_LXQT_TERMINAL))
    res.append(ctn_LXQT_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_KDE_TERMINAL))
    res.append(ctn_KDE_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_TDE_TERMINAL))
    res.append(ctn_TDE_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_CINNAMON_TERMINAL))
    res.append(ctn_CINNAMON_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_MATE_TERMINAL))
    res.append(ctn_MATE_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_RXVT_TERMINAL))
    res.append(ctn_RXVT_TERMINAL);

  if (UnixCommand::hasTheExecutable(ctn_XTERM))
    res.append(ctn_XTERM);

  res.append(ctn_QTERMWIDGET);
  res.removeDuplicates();
  res.sort();

  return res;
}*/
