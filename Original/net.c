#include "net.h"
#include "Ui.h"

gpointer net_UdpBroadcaster(gpointer data)
{
	GSocket *NewSocket;
	GError *Error = NULL;
	GInetAddress *MulticastAddress = NULL;
	GSocketAddress *SocketAddress = NULL;
	gssize Bytes;
	gchar Buffer[BUFFERSIZE];

	gMagicNumber = g_random_int_range(0, 1000000);
	g_printf("MagicNumber is %d\n",gMagicNumber);

	NewSocket = net_CreateSocket(G_SOCKET_TYPE_DATAGRAM);

	//send and join multicast group 
	MulticastAddress = net_JoinToMulticast(NewSocket);
	g_socket_set_multicast_loopback(NewSocket, FALSE);

	if((SocketAddress = g_inet_socket_address_new(MulticastAddress, PORT)) == NULL)
	{
#ifdef DEBUG
		g_printf("no parsable address\n");
#endif
		exit(1);
	}

	g_sprintf(Buffer,"%s%d",STARTMSG, gMagicNumber);
	for(;;)
	{
		g_usleep(5* 1000000);
		if((Bytes = g_socket_send_to(NewSocket, SocketAddress, Buffer, strlen(Buffer), NULL, &Error)) == -1)
		{
#ifdef DEBUG
		g_printf("send error: %s\n", Error->message);
#endif
		}
	}
}

gpointer net_UdpServer(gpointer data)
{
	GSocket *NewSocket;
	GError *Error = NULL;
	GSocketAddress *ClientAddressStructure = NULL;
	GInetAddress *ClientAddress;
	//GInetAddress *MulticastAddress = NULL;
	gssize Bytes;
	gchar Buffer[BUFFERSIZE];
	GList *TmpList;

	gchar* RemoteIpAddressString;
	gchar* LocalIpAddressString;

	gint RemoteMagicNumber;


	g_usleep(2 * 1000000);
	NewSocket = net_CreateSocketAndBind(G_SOCKET_TYPE_DATAGRAM);

	/*MulticastAddress = */net_JoinToMulticast(NewSocket);
	g_socket_set_multicast_loopback(NewSocket, FALSE);
	for(;;)
	{
		if((Bytes = g_socket_receive_from(NewSocket,
						&ClientAddressStructure,
						Buffer,
						sizeof(Buffer),
						NULL,
						&Error)) <= 0)
		{
#ifdef DEBUG
			g_printf("recv error:%s\n", Error->message);
#endif
			exit(1);
		}

		Buffer[Bytes] = '\0';
		ClientAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(ClientAddressStructure));
		RemoteIpAddressString = g_inet_address_to_string(ClientAddress);


		if(g_list_find_custom(ListOfUsedAddresses,
					RemoteIpAddressString,
					net_CompareFunc) == NULL)
		{
			LocalIpAddressString = GetLocalAddressString(NewSocket);
			g_printf("incoming message: %s\n", Buffer);
			if(g_str_has_prefix(Buffer, STARTMSG))
			{	
//
				g_mutex_lock(&SharedMutex);
#ifdef DEBUG
				g_printf("mutex locked in UdpReciever\n");
#endif
				//g_object_ref(ClientAddress);
				TmpList = ListOfUsedAddresses;
				ListOfUsedAddresses = g_list_prepend(ListOfUsedAddresses, RemoteIpAddressString);

				g_printf("items in list: %d\n", g_list_length(ListOfUsedAddresses));

				if(ListOfUsedAddresses == TmpList)
				{
#ifdef DEBUG
					g_printf("Adding to list error\n");
#endif
				}
				g_mutex_unlock(&SharedMutex);
#ifdef DEBUG
				g_printf("mutex received in UdpReciever with addition\n");
#endif
#ifdef DEBUG
				g_printf("\033[0;32m UdpServer:\033[0m Recieved message \"%s\" of %"G_GSSIZE_FORMAT" bytes from %s\n", 
						Buffer, Bytes, RemoteIpAddressString);
#endif
					
//
				RemoteMagicNumber = GetMagicNumber(Buffer);
				if(CompareMagicNumbers(RemoteMagicNumber, LocalIpAddressString, RemoteIpAddressString) == TRUE)
				{
					if(net_ClientFactory((gpointer)ClientAddress, RemoteMagicNumber) == NULL)
					{
						g_mutex_lock(&SharedMutex);
						ListOfUsedAddresses = g_list_remove(ListOfUsedAddresses, RemoteIpAddressString);
						g_mutex_unlock(&SharedMutex);
					}
				}
			}
			g_free(LocalIpAddressString);
		}
		//g_free(RemoteIpAddressString);

#ifdef DEBUG
		//g_printf("\033[0;32m UdpServer:\033[0m %d position in List\n", g_list_length(ListOfUsedAddresses));
#endif
	}
}

gpointer net_TcpReciever(gpointer data)
{
	GSocket *NewSocket;
	GSocket *IncomingSocket;
	GInetAddress *ClientAddress;
	GError *Error = NULL;
	gchar Buffer[BUFFERSIZE];
	int RecievedMagicNumber;

	g_usleep(2 * 1000000);
	NewSocket = net_CreateSocketAndBind(G_SOCKET_TYPE_STREAM);

	if(g_socket_listen(NewSocket, &Error) == FALSE)
	{
#ifdef DEBUG
	g_printf("listen error: %s\n", Error->message);
#endif
		exit(1);
	}
	for(;;)
	{
#ifdef DEBUG
		printf("\033[0;33m Tcp Server: \033[0m Listen ok. Waiting\n");
#endif
		if((IncomingSocket = g_socket_accept(NewSocket, NULL, &Error)) == NULL)
		{
#ifdef DEBUG
			g_printf("accept error: %s\n", Error->message);
#endif
			exit(1);
		}
		ClientAddress = net_GetInetAddressFromGSocket(IncomingSocket);

#ifdef DEBUG
		printf("\033[0;33m Tcp Server: \033[0m incoming to TcpServer from %s\n",
			       g_inet_address_to_string(ClientAddress));
#endif
		net_RecieveMessage(IncomingSocket, Buffer);
		if(g_strcmp0(Buffer, "NoData") == 0)
		{
			g_printf("errrr\n");
			return NULL;
		}
		RecievedMagicNumber = GetMagicNumber(Buffer);
		if(g_str_has_prefix(Buffer, ESTABMSG) &&  RecievedMagicNumber == gMagicNumber)
		{
			g_printf("Recieved MagicNumber is matching\n");
			net_TcpResponder((gpointer)IncomingSocket);
		}
		//else
		{
			//g_mutex_unlock(&SharedMutex);
#ifdef DEBUG
		g_printf("mutex received in TcpReciever with escape\n");
#endif
#ifdef DEBUG
		g_printf("\033[0;33m Tcp Server: \033[0m connection rejected\n");
#endif
		}
	}
}

GSocket* net_CreateSocketAndBind(GSocketType SockType)
{
	GSocket *NewSocket;
	GError *Error = NULL;
	GSocketAddress *Address = NULL;
	GInetAddress *InetAddress = NULL;
	gchar* IpAddressString;
	//GInetAddressMask* adM = NULL;
	//GInetAddress* adre = NULL;

	NewSocket = net_CreateSocket(SockType);
	//address
	InetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
	//InetAddress = g_inet_address_new_from_string("172.17.190.33");
	if((Address = g_inet_socket_address_new(InetAddress,PORT)) == NULL)
	{
#ifdef DEBUG
		g_printf("no parsable address\n");
#endif
		exit(1);
	}
	//adre = g_inet_socket_address_get_address(InetAddress);

	//adM = g_inet_address_mask_new(InetAddress,32,&Error);
	//g_printf("address: %s\n",  g_inet_address_mask_to_string(adM));

	//bind
	if(g_socket_bind(NewSocket, Address, TRUE, &Error) == 0)
	{
#ifdef DEBUG
		g_printf("bind error:%s\n", Error->message);
#endif
		exit(1);
	}

	IpAddressString = GetLocalAddressString(NewSocket);
	g_printf("address: %s\n", IpAddressString);

	return NewSocket;
}

GInetAddress* net_GetInetAddressFromGSocket(GSocket *localSocket)
{
	GInetAddress *ClientAddress;
	GSocketAddress *SockAddr;
	GError *Error = NULL;
	if((SockAddr = g_socket_get_remote_address(localSocket, &Error)) == NULL)
	{
#ifdef DEBUG
		g_printf("error:%s\n", Error->message);
#endif
		exit(1);
	}
	ClientAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(SockAddr));
	//g_object_unref(SockAddr);
	g_object_ref(ClientAddress);
	return ClientAddress;
}

gpointer net_ClientFactory(gpointer Address, gint NumberSend)
{
	GInetAddress *lAddress = G_INET_ADDRESS(Address);
	GSocket *NewSocket;
	GError *Error = NULL;
	GSocketAddress* lSockAddress;
	gchar Buffer[BUFFERSIZE];
	gchar *OutBuffer;
	gchar *ServerAddress;
	gchar *TmpBuffer = NULL;
	GSource* src;

	ServerAddress = g_inet_address_to_string(lAddress);
	NewSocket = net_CreateSocket(G_SOCKET_TYPE_STREAM);

	lSockAddress = g_inet_socket_address_new(lAddress, PORT);
	if((g_socket_connect(NewSocket, lSockAddress, NULL, &Error)) == FALSE)
	{
#ifdef DEBUG
		g_printf("connect error: %s\n",Error->message);
#endif
		g_object_unref(lSockAddress);
		return NULL;
	}
	g_object_unref(lSockAddress);
#ifdef DEBUG
	printf("\033[0;35m Tcp Client: \033[0m TcpClient socket to server %s created \n", ServerAddress);
#endif
	//Send Magic Number
	g_sprintf(Buffer,"%s%d",ESTABMSG, NumberSend);
	net_SendMessage(NewSocket, Buffer);
	///Recieve
	net_RecieveMessage(NewSocket, Buffer);
	if(g_strcmp0(Buffer, "NoData") == 0)
	{
		g_printf("errrr\n");
		return NULL;
	}

	//Send username
	TmpBuffer = g_strconcat("120-", g_get_user_name(), NULL);
	OutBuffer = CreateUtf8String(TmpBuffer);
	g_free(TmpBuffer);
	g_printf("end create string\n");
	net_SendMessage(NewSocket, OutBuffer);
	if(g_strcmp0(OutBuffer, "NoData") == 0)
	{
		g_free(OutBuffer);
		return NULL;
	}

	//Recieve username
	net_RecieveMessage(NewSocket, Buffer);
	if(g_strcmp0(Buffer, "NoData") == 0)
			return NULL;
	
	//Fill structure
	ConnectionStructure* ConStr = g_new(ConnectionStructure, 1);
#ifdef DEBUG
	g_printf("Buffer before Check - %s\n", Buffer);
#endif
	ConStr->UserName = CheckUserName(Buffer);
	
	ConStr->Socket = NewSocket;
	ConStr->AddressString = ServerAddress;
	//ConStr->UserName = g_strdup("server");

	g_main_context_invoke(NULL, (GSourceFunc)DialogCreation, ConStr);
	ConnectionData = g_list_prepend(ConnectionData, ConStr);

	g_socket_set_blocking(NewSocket, FALSE);
	src = g_socket_create_source(NewSocket, G_IO_IN, NULL);
	ConStr->Source = src;
	g_source_set_callback(src, (GSourceFunc)SocketCallback, ConStr, NULL);
	g_source_attach(src, NULL);
	g_source_unref(src);

	return (gpointer)NewSocket;
}

gpointer net_TcpResponder(gpointer data)
{
	GSocket* InSocket; 
	gchar InBuffer[BUFFERSIZE];
	gchar *OutBuffer;
	gchar *ClientAddress;
	gchar* TmpBufferX = NULL;
	GSource* src;

	InSocket = G_SOCKET(data);
	
	ClientAddress = g_inet_address_to_string(net_GetInetAddressFromGSocket(InSocket));

	OutBuffer = CreateUtf8String("100-Connection Ok");
	net_SendMessage(InSocket, OutBuffer);
	if(g_strcmp0(OutBuffer, "NoData") == 0)
	{
		return NULL;
	}
	g_free(OutBuffer);
#ifdef DEBUG
	//printf("\033[0;34m Tcp ServerResponse: \033[0m Sent :%"G_GSSIZE_FORMAT" bytes to client\n", Bytes);
#endif

	net_RecieveMessage(InSocket, InBuffer);
	if(g_strcmp0(InBuffer, "NoData") == 0)
	{
		g_free(InBuffer);
		return NULL;
	}
	TmpBufferX = g_strconcat("120-", g_get_user_name(), NULL);
	OutBuffer = CreateUtf8String(TmpBufferX);
	g_free(TmpBufferX);
	net_SendMessage(InSocket, OutBuffer);
	if(g_strcmp0(OutBuffer, "NoData") == 0)
	{
		return NULL;
	}
	g_free(OutBuffer);

	ConnectionStructure* ConStr = g_new(ConnectionStructure, 1);
	ConStr->Socket = InSocket;
	ConStr->UserName = CheckUserName(InBuffer);
	ConStr->AddressString = ClientAddress;

	g_main_context_invoke(NULL, (GSourceFunc)DialogCreation, ConStr);
	ConnectionData = g_list_prepend(ConnectionData, ConStr);

	g_socket_set_blocking(InSocket, FALSE);
	src = g_socket_create_source(InSocket, G_IO_IN, NULL);
	ConStr->Source = src;
	g_source_set_callback(src, (GSourceFunc)SocketCallback, ConStr, NULL);
	g_source_attach(src, NULL);
	g_source_unref(src);

	return (gpointer)InSocket;
}

gint net_CompareFunc(gconstpointer a, gconstpointer b)
{
	return g_strcmp0(a, b);
}

GInetAddress* net_JoinToMulticast(GSocket* lSocket)
{
	GInetAddress *MulticastAddress;
	GError *Error = NULL;
	if((MulticastAddress = g_inet_address_new_from_string(MLTCSTGRP)) == NULL)
	{
#ifdef DEBUG
		g_printf("no parsable address\n");
#endif
		exit(1);
	}
	if(g_socket_join_multicast_group(lSocket, MulticastAddress, FALSE, NULL, &Error) == FALSE)
	{
#ifdef DEBUG
		g_printf("multicast group join error\n");
#endif
		exit(1);
	}
	return MulticastAddress;
}

int net_StartPoint()
{
	GError *Error = NULL;
	GThread *Thread[3];
	ListOfUsedAddresses = NULL;
	//init mutex
	g_mutex_init(&SharedMutex);

	Thread[1] = g_thread_try_new("1", net_UdpServer, NULL, &Error);
	if(Thread[1] == NULL)
	{
#ifdef DEBUG
		g_printf("thread error: %s\n", Error->message);
#endif
	}
	Thread[0] = g_thread_try_new("2", net_UdpBroadcaster, NULL, &Error);
	if(Thread[0] == NULL)
	{
#ifdef DEBUG
		g_printf("thread error: %s\n", Error->message);
#endif
	}
	Thread[2] = g_thread_try_new("3", net_TcpReciever, NULL, &Error);
	if(Thread[0] == NULL)
	{
#ifdef DEBUG
		g_printf("thread error: %s\n", Error->message);
#endif
	}

	/*
	for(i=0;i<3;i++)
		g_thread_join(Thread[i]);
	*/
	return 0;
}

gchar* CreateUtf8String(gchar* str)
{
#ifdef DEBUG
	g_printf("utf8 input: %s\n", str);
#endif
	gchar *Buffer;
	const gchar *rBuff;
	Buffer = g_strdup(str);
	if(g_utf8_validate(Buffer, -1, &rBuff) == FALSE)
	{
		Buffer[Buffer - rBuff] = '\0';
#ifdef DEBUG
		g_printf("utf8 validate: %s\n", str);
#endif

		/*
		g_printf("not valid string\n");
		rBuff = g_locale_to_utf8(Buffer, strlen(Buffer), NULL, NULL, NULL);
		g_printf("utf8 validate: %s\n", str);
		//g_free(Buffer);
		Buffer = rBuff;
		//g_stpcpy(Buffer, rBuff);
		*/
	}
#ifdef DEBUG
	g_printf("utf8 output: %s\n", Buffer);
#endif
	return Buffer;
}

gchar* CheckUserName(gchar* Uname)
{
#ifdef DEBUG
	g_printf("string in CheckUserName %s\n", Uname);
#endif
	gchar* Buffer;
	gchar* Pnt;
	if(g_str_has_prefix(Uname, "120-") == TRUE)
	{	
		if(g_utf8_validate(Uname, -1, NULL))
		{
			Pnt = Uname + strlen("120-");
			Buffer = g_strdup(Pnt);
			return Buffer;	
		}
	}
	return Buffer = g_strdup("Unknown");
}

void net_RecieveMessage(GSocket* NewSocket, gchar* Data)
{
#ifdef DEBUG
	g_printf("enter recieve message f\n");
#endif
	gssize Bytes;
	GError *Error = NULL;
	gchar Buffer[BUFFERSIZE];
	/*
	while((Bytes  = g_socket_receive(NewSocket, Buffer, sizeof(Buffer), NULL, &Error)) > 0)
	{
		g_printf("recieved %s\n", Buffer);
		Pnt = g_stpcpy(Pnt, Buffer);
	}
	*/
	if((Bytes = g_socket_receive(NewSocket, Buffer, sizeof(Buffer), NULL, &Error)) <= 0)
	{
		g_printf("%s\n",Error->message);
		g_object_unref(NewSocket);
		g_stpcpy(Buffer, "NoData");
	}
	Buffer[Bytes] = '\0';
	g_stpcpy(Data, Buffer);
#ifdef DEBUG
	//printf("\033[0;35m Tcp Client: \033[0m Recieved message \"%s\": %"G_GSSIZE_FORMAT" bytes from server\n", Buffer, Bytes);
	printf("\033[0;35m Tcp: \033[0m Recieved message \"%s\": %"G_GSSIZE_FORMAT" bytes \n", Buffer, Bytes);
#endif
}

void net_SendMessage(GSocket* NewSocket, gchar* OutBuffer)
{
#ifdef DEBUG
	g_printf("send message %s\n", OutBuffer);
#endif
	gssize Bytes;
	GError *Error = NULL;
	if((Bytes = g_socket_send(NewSocket, OutBuffer, strlen(OutBuffer), NULL, &Error)) == -1)
	{
#ifdef DEBUG
		g_printf("send message error: %s\n", Error->message);
#endif
		g_object_unref(NewSocket);
		g_stpcpy(OutBuffer, "NoData");
	}
}

GSocket* net_CreateSocket(GSocketType SockType)
{
	GSocket* NewSocket;
	GError *Error = NULL;

	NewSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, SockType, 0, &Error);
	if(NewSocket == NULL)
	{
#ifdef DEBUG
		g_printf("socket error: %s\n", Error->message);
#endif
		exit(1);
	}
	return NewSocket;
}

gboolean SocketCallback(GSocket *socket, GIOCondition cond, gpointer user_data)
{
	GSource* CurSrc;

	gchar Buffer[BUFFERSIZE];
	gssize Bytes;

	CurSrc = (GSource*)(((ConnectionStructure*)user_data)->Source);
	g_printf("recieved condition :%d\n",cond);
	if(cond == G_IO_IN)
	{
#ifdef DEBUG
		g_printf("message incoming from %s@%s\n",
				((ConnectionStructure*)user_data)->UserName, ((ConnectionStructure*)user_data)->AddressString);
#endif
		if((Bytes = g_socket_receive(socket, Buffer, sizeof(Buffer), NULL, NULL)) == 0)
		{
#ifdef DEBUG
			g_print("connection closed by peer\n");	
#endif

			DialogRemove((ConnectionStructure*)user_data);
			//return FALSE;
		}
		else
		{
			Buffer[Bytes] = '\0';
			HandleInputMessage((ConnectionStructure*)user_data, Buffer);
			return TRUE;
		}
	}
	else if((cond ^ G_IO_ERR) < cond || (cond ^ G_IO_HUP) < cond || (cond ^ G_IO_NVAL) < cond)
	{
		DialogRemove((ConnectionStructure*)user_data);
		g_printf("socket error\n");
		//return FALSE;
	}
	else
		g_printf("another %d\n",cond);

	//g_printf("source %s destroyed\n", g_source_is_destroyed(CurSrc) ? "is" : "is not");
	g_source_destroy(CurSrc);
	//g_printf("source %s destroyed\n", g_source_is_destroyed(CurSrc) ? "is" : "is not");
	//g_source_unref(CurSrc); //g_source_set_ready_time: assertion 'source->ref_count > 0' failed
	return FALSE;
}

int GetMagicNumber(gchar* IncomingBuffer)
{
	gint MagicNumberStringLen;
	gchar* MagicNumberString;
	gint RemoteMagicNumber;

	MagicNumberStringLen = strlen(STARTMSG);
	MagicNumberString = &(IncomingBuffer[MagicNumberStringLen]);

	if((RemoteMagicNumber = g_ascii_strtoll(MagicNumberString, NULL, 10)) == 0)
	{
		g_printf("no magic number recieved\n");
	}

	return RemoteMagicNumber;
}

gboolean CompareMagicNumbers(gint  RemoteMagicNumber, gchar* LocalIpAddress, gchar* RemoteIpAddress)
{
	gchar* remString;
	gchar* locString;

	gint RemoteIntAddress;
	gint LocalIntAddress;

	if(gMagicNumber < RemoteMagicNumber)
	{
		g_printf("Local MagicNumber less than\n");
		return TRUE;
	}
	else if(gMagicNumber > RemoteMagicNumber)
	{
		g_printf("Remote MagicNumber less than\n");
		return FALSE;
	}

	if((remString = g_strrstr(RemoteIpAddress, ".")) == NULL)
	{
		g_printf("unparceble remAddr string\n");
		return FALSE;
	}

	if((locString = g_strrstr(LocalIpAddress, ".")) == NULL)
	{
		g_printf("unparceble locAddr string\n");
		return FALSE;
	}

	if((RemoteIntAddress = g_ascii_strtoll(remString + 1, NULL, 10)) == 0)
	{
		g_printf("ascii_strtool rem\n");
		return FALSE;
	}
	if((LocalIntAddress = g_ascii_strtoll(locString + 1, NULL, 10)) == 0)
	{
		g_printf("ascii_strtool loc\n");
		return FALSE;
	}

	if(LocalIntAddress < RemoteIntAddress)
		return TRUE;
	else
		return FALSE;
}

gchar* GetLocalAddressString(GSocket* localSocket)
{
	GError *Error = NULL;
	GSocketAddress *LocalAddressStructure = NULL;
	GInetAddress *LocalInetAddress;
	gchar* LocalIpAddressString;
	
	if((LocalAddressStructure = g_socket_get_local_address(localSocket, &Error)) == NULL)
	{
		g_printf("GetLocalAddressString func:%s\n", Error->message);
	}

	LocalInetAddress = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(LocalAddressStructure));
	LocalIpAddressString = g_inet_address_to_string(LocalInetAddress);


	g_object_unref(LocalAddressStructure);
	return LocalIpAddressString;
}
