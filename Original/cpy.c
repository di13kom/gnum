gsize Size;
gsize Total = 0;
gsize StrTextLen;
gchar OutPutString[BUFFERSIZE];
gint i=0;

StrTextLen = strlen(Text);
if(StrTextLen > (BUFFERSIZE - 1))
{
	while(1)
	{
		while(StrTextLen < (i * BUFFERSIZE))
		{
			g_strlcpy(OutPutString, Text[(i * BUFFERSIZE) - i], BUFFERSIZE);
			i++;
			if(g_socket_condition_wait(Sock, G_IO_OUT, NULL, NULL) == TRUE)
				net_SendMessage(Sock, OutPutString);
		}
	}
}



PrepareMessageToSend(gchar* Text)
{
	gsize StrTextLen;
	gchar OutPutString[BUFFERSIZE];
	gint i=0;

	//send prefix
	g_strlcpy(OutPutString, "220-", BUFFERSIZE);
		if(g_socket_condition_wait(Sock, G_IO_OUT, NULL, NULL) == TRUE)
			net_SendMessage(Sock, OutPutString);
	StrTextLen = strlen(Text);
	while(StrTextLen < (i * BUFFERSIZE))
	{
		g_strlcpy(OutPutString, &(Text[(i * BUFFERSIZE) - i]), BUFFERSIZE);
		i++;
		if(g_socket_condition_wait(Sock, G_IO_OUT, NULL, NULL) == TRUE)
			net_SendMessage(Sock, OutPutString);
	}
	//send suffix
	g_strlcpy(OutPutString, "-220", BUFFERSIZE);
		if(g_socket_condition_wait(Sock, G_IO_OUT, NULL, NULL) == TRUE)
			net_SendMessage(Sock, OutPutString);
}
