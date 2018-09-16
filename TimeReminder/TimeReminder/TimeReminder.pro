#-------------------------------------------------
#
# Project created by QtCreator 2018-09-09T13:29:32
#
#-------------------------------------------------
include($$PWD/../Base.pri)

QT       += core gui widgets axcontainer sql

TARGET = TimeReminder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        MainWindow.cpp \
    DatePick.cpp \
    AddDialog.cpp \
    RecordEntity.cpp

HEADERS += \
        MainWindow.h \
    DatePick.h \
    AddDialog.h \
    RecordEntity.h

FORMS += \
        mainwindow.ui \
    datepick.ui \
    adddialog.ui


LIBS += -L$$DESTDIR \
        -lExcelOperate \
        -lDbOperate

INCLUDEPATH += $$PWD/../ExcelOperate \
                $$PWD/../DbOperate


DEPENDPATH += $$PWD/../ExcelOperate \
                $$PWD/../DbOperate

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)

#QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
