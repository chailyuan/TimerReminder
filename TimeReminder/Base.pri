DESTDIR = $$PWD/Run/bin

CONFIG += C++11

CONFIG(debug, debug|release){
DESTDIR = $$join(DESTDIR,,,_debug)
}
