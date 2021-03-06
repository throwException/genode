QT_DEFINES += -DQT_VIRTUALKEYBOARD_DEFAULT_STYLE=\"default\" -DQT_VIRTUALKEYBOARD_DESKTOP -DQT_NO_EXCEPTIONS -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -DQT_NO_DEBUG -DQT_PLUGIN -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QML_LIB -DQT_NO_QML_DEBUGGER -DQT_CORE_LIB

QT_INCPATH += \
  qtvirtualkeyboard/src/virtualkeyboard \
  qtdeclarative/include \
  qtdeclarative/include/QtQuick \
  qtbase/include/QtGui/5.8.0 \
  qtbase/include/QtGui/5.8.0/QtGui \
  qtbase/include \
  qtbase/include/QtGui \
  qtdeclarative/include/QtQml \
  qtbase/include/QtNetwork \
  qtbase/include/QtCore/5.8.0 \
  qtbase/include/QtCore/5.8.0/QtCore \
  qtbase/include/QtCore \
  qtbase/mkspecs/genode-g++

QT_SOURCES += \
  platforminputcontext.cpp \
  inputcontext.cpp \
  abstractinputmethod.cpp \
  plaininputmethod.cpp \
  inputengine.cpp \
  shifthandler.cpp \
  plugin.cpp \
  inputmethod.cpp \
  selectionlistmodel.cpp \
  defaultinputmethod.cpp \
  abstractinputpanel.cpp \
  enterkeyaction.cpp \
  enterkeyactionattachedtype.cpp \
  settings.cpp \
  virtualkeyboardsettings.cpp \
  trace.cpp \
  desktopinputpanel.cpp \
  inputview.cpp \
  appinputpanel.cpp \
  qrc_default_style.cpp \
  qrc_retro_style.cpp \
  qrc_content.cpp \
  qrc_layouts.cpp \
  moc_platforminputcontext.cpp \
  moc_inputcontext.cpp \
  moc_abstractinputmethod.cpp \
  moc_plaininputmethod.cpp \
  moc_inputengine.cpp \
  moc_shifthandler.cpp \
  moc_inputmethod.cpp \
  moc_selectionlistmodel.cpp \
  moc_defaultinputmethod.cpp \
  moc_abstractinputpanel.cpp \
  moc_enterkeyaction.cpp \
  moc_enterkeyactionattachedtype.cpp \
  moc_settings.cpp \
  moc_virtualkeyboardsettings.cpp \
  moc_plugin.cpp \
  moc_trace.cpp \
  moc_desktopinputpanel.cpp \
  moc_inputview.cpp \
  moc_appinputpanel.cpp

QT_VPATH += \
  qtvirtualkeyboard/src/virtualkeyboard \

# some source files need to be generated by moc from other source/header files before
# they get #included again by the original source file in the compiling stage

# source files generated from existing header files ('moc_%.cpp: %.h' rule in import-qt5.inc)
# extracted from 'compiler_moc_header_make_all' target

COMPILER_MOC_HEADER_MAKE_ALL_FILES = \
  moc_platforminputcontext.cpp \
  moc_inputcontext.cpp \
  moc_abstractinputmethod.cpp \
  moc_plaininputmethod.cpp \
  moc_inputengine.cpp \
  moc_shifthandler.cpp \
  moc_inputmethod.cpp \
  moc_selectionlistmodel.cpp \
  moc_defaultinputmethod.cpp \
  moc_abstractinputpanel.cpp \
  moc_enterkeyaction.cpp \
  moc_enterkeyactionattachedtype.cpp \
  moc_settings.cpp \
  moc_virtualkeyboardsettings.cpp \
  moc_plugin.cpp \
  moc_trace.cpp \
  moc_desktopinputpanel.cpp \
  moc_inputview.cpp \
  moc_appinputpanel.cpp

# source files generated from existing source files ('%.moc: %.cpp' rule in import-qt5.inc)
# extracted from 'compiler_moc_source_make_all' rule

COMPILER_MOC_SOURCE_MAKE_ALL_FILES = \
 

