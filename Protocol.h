#define STARTMSG "150"
#define ESTABMSG "101"

#define REMOTE_UNAME 200

#define FILE_REQUEST "500"
#define FILE_ACCEPT "550"
#define FILE_DENY "580"

enum ProMessage
{
	Estab_Msg = 101,
	Start_Msg = 150,

	Uname_Msg = 200,

	fRequest_Msg = 500,
	fAccept_Msg = 550,
	fDeny_Msg = 580
};
