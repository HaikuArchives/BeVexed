#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <Locker.h>
#include <Message.h>
#include <File.h>
#include <String.h>
#include <InterfaceDefs.h>

extern BLocker prefsLock;
extern BMessage gPreferences;
extern BString gAppPath;

status_t SavePreferences(const char *path);
status_t LoadPreferences(const char *path);

void ConstrainWindowFrameToScreen(BRect *rect);

#endif
