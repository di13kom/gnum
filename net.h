#ifndef Net_H
#define Net_h
#include <glib.h>
#include <gio/gio.h>
#include <glib/gprintf.h>
#include <string.h>
#include <stdlib.h>
#define PORT 6060
#define BUFFERSIZE 500
#define MLTCSTGRP "224.0.0.1"

#define STARTMSG "150-"
#define ESTABMSG "101-"
#define FILE_REQUEST_START "500-"
#define FILE_REQUEST_END "-500"

#define HEADERLEN strlen(STARTMSG)

gint gMagicNumber;

int net_StartPoint();

gpointer net_UdpBroadcaster(gpointer);
gpointer net_UdpServer(gpointer);
gpointer net_TcpReciever(gpointer);

GSocket* net_CreateSocketAndBind(GSocketType);
GInetAddress* net_GetInetAddressFromGSocket(GSocket *);
gpointer net_ClientFactory(gpointer, gint);
gpointer net_TcpResponder(gpointer);
gint net_CompareFunc(gconstpointer, gconstpointer);
GInetAddress* net_JoinToMulticast(GSocket*);
GSocket* net_CreateSocket(GSocketType);

gchar* CreateUtf8String(gchar*);
gchar* CheckUserName(gchar*);
void net_RecieveMessage(GSocket*, gchar*);
void net_SendMessage(GSocket*, gchar*);

gboolean SocketCallback(GSocket*, GIOCondition, gpointer);
gchar* GetLocalAddressString(GSocket*);
gboolean CompareMagicNumbers(gint, gchar*, gchar*);
int GetMagicNumber(gchar*);

GList *ListOfUsedAddresses;
GMutex SharedMutex;
#endif
