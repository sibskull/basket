/***************************************************************************
 *   Copyright (C) 2003 by Sébastien Laoût                                 *
 *   slaout@linux62.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "application.h"

#include <KCmdLineArgs>
//#include <KDebug>

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

#include "global.h"
#include "bnpview.h"

#ifdef WITH_LIBGIT2
extern "C" {
#include <git2.h>
}
#endif

Application::Application(int &argc, char **argv)
        : QApplication(argc, argv)
{
    QCoreApplication::setApplicationName(i18n("Basket"));
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setOrganizationDomain("kde.org");
    QApplication::setApplicationDisplayName(i18n("BasKet Note Pads"));
    //BasketPart::createAboutData();
    KDBusService service(KDBusService::Unique); //make global

    newInstance();


    #ifdef WITH_LIBGIT2
        git_threads_init();
    #endif
}

Application::~Application()
{
    #ifdef WITH_LIBGIT2
        git_threads_shutdown();
    #endif
}

int Application::newInstance()
{
    //KUniqueApplication::newInstance();

    // Open the basket archive or template file supplied as argument:
    if (args && parser.positionalArguments().count() >= 1) {
        QString fileName = args->arg(parser.positionalArguments().count() - 1);

        if (QFile::exists(fileName)) {
            QFileInfo fileInfo(fileName);
            if (fileInfo.absoluteFilePath().contains(Global::basketsFolder())) {
                QString folder = fileInfo.absolutePath().split("/").last();
                folder.append("/");
                BNPView::s_basketToOpen = folder;
                QTimer::singleShot(100, Global::bnpView, SLOT(delayedOpenBasket()));
            } else if (!fileInfo.isDir()) { // Do not mis-interpret data-folder param!
                // Tags are not loaded until Global::bnpView::lateInit() is called.
                // It is called 0ms after the application start.
                BNPView::s_fileToOpen = fileName;
                QTimer::singleShot(100, Global::bnpView, SLOT(delayedOpenArchive()));
//              Global::bnpView->openArchive(fileName);
                
            }
        }
    }
    return 0;
}
