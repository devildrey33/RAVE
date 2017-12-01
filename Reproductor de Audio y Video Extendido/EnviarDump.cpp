#include "stdafx.h"
#include "EnviarDump.h"

#include <locale> 
#include <codecvt>
#include <Ws2tcpip.h>
#include "RAVE_CuentaEmail.h"

#pragma comment(lib, "ws2_32.lib")


EnviarDump::EnviarDump(void) : _Error(FALSE), _Server(NULL) {
}


EnviarDump::~EnviarDump(void) {
}

// Enviar sin mostrar por la consola
const BOOL EnviarDump::_EnviarNC(const char *eTxt) {
	std::string		Texto = eTxt + std::string("\r\n");
	int iStatus = send(_Server, Texto.c_str(), static_cast<int>(Texto.size()), 0);
	if ((iStatus != SOCKET_ERROR) && (iStatus)) {
		return FALSE;
	}
	_Error = TRUE;
	return TRUE;
}

const BOOL EnviarDump::_Enviar(const char *eTxt, const BOOL nRecibir) {
	std::string		Texto = eTxt; // +std::string("\r\n");
#ifdef MOSTRAR_CONSOLA
	size_t			out;
	static wchar_t	WBuffer[4096];
	ZeroMemory(WBuffer, sizeof(wchar_t) * 4096);
	mbstowcs_s(&out, WBuffer, 4096, Texto.c_str(), Texto.size());
	Debug_Escribir_Varg(L"EnviarDump::_Enviar %s\n", WBuffer);
#endif
	Texto += "\r\n";
	int iStatus = send(_Server, Texto.c_str(), static_cast<int>(Texto.size()), 0);
	if ((iStatus != SOCKET_ERROR) && (iStatus)) {
		if (nRecibir == TRUE)	return _Recibir();
		else					return FALSE;
	}
	_Error = TRUE;
	return TRUE;
}

const BOOL EnviarDump::_Recibir(void) {
	static char Buffer[4096];
	ZeroMemory(Buffer, sizeof(char) * 4096);
	int iStatus = recv(_Server, Buffer, 4096, 0);
#ifdef MOSTRAR_CONSOLA
	std::string		Texto = Buffer;
	size_t			out;
	static wchar_t	WBuffer[4096];
	ZeroMemory(WBuffer, sizeof(wchar_t) * 4096);
	mbstowcs_s(&out, WBuffer, 4096, Texto.c_str(), Texto.size());
	Debug_Escribir_Varg(L"EnviarDump::_Recibir %s\n", WBuffer);
#endif
	if ((iStatus != SOCKET_ERROR) && (iStatus))	return FALSE;
	_Error = TRUE;
	return TRUE;
}


const BOOL EnviarDump::Enviar(std::wstring &Path, DWL::DBarraProgresoEx &Barra) {
	int         iProtocolPort			= 0;
	char        ServidorSmtp[64]		= EMAIL_SMTP;
	char        CorreoDestino[64]		= EMAIL_DESTINO;
	char        CorreoOrigen[64]        = EMAIL_ORIGEN;
	char        CorreoOrigen_Login[]    = EMAIL_LOGIN;
	char        CorreoOrigen_Password[] = EMAIL_PASS;
	char        szMsgLine[256]			= "";
	
	
//	SOCKET      hServer					= NULL;
	WSADATA     WSData;
//	LPHOSTENT   lpHostEntry;
	LPSERVENT   lpServEntry;
	SOCKADDR_IN SockAddr;


	struct addrinfo *result = NULL;
	struct addrinfo *p = NULL;
	struct addrinfo hints;
	struct sockaddr_in *remote;
	char ip_addr[INET6_ADDRSTRLEN];

	_Error = FALSE;

	// Attempt to intialize WinSock (1.1 or later)
	if (WSAStartup(MAKEWORD(2, 2), &WSData)) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error cargando Winsock.\n");
		WSACleanup();
		return FALSE;
	}

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	DWORD dwRetval = getaddrinfo(ServidorSmtp, "smtp", &hints, &result);

	for (p = result; p != NULL; p = p->ai_next) {
//		void *addr;
		remote = (struct sockaddr_in *)p->ai_addr;
		//addr = &(remote->sin_addr);

		// Convert IP to string
		inet_ntop(p->ai_family, &remote->sin_addr, ip_addr, sizeof(ip_addr));
		SockAddr.sin_addr = remote->sin_addr;
//		printf("%s\n", ip_addr);
	}

/*	// Lookup email server's IP address.
	lpHostEntry = gethostbyname(szSmtpServerName);
	if (!lpHostEntry) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error obteniendo la ip del servidor de email.\n");
		WSACleanup();
		return FALSE;
	}*/

	// Create a TCP/IP socket, no specific protocol
	_Server = socket(PF_INET, SOCK_STREAM, 0);
	if (_Server == INVALID_SOCKET) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error creando el socket.\n");
		WSACleanup();
		return FALSE;
	}

	// Get the mail service port
	lpServEntry = getservbyname("mail", 0);

	// Use the SMTP default port if no other port is specified
	if (!lpServEntry)		iProtocolPort = htons(IPPORT_SMTP);
	else					iProtocolPort = lpServEntry->s_port;

	//inet_pton(AF_INET, ip_addr, Bufer);
	// Setup a Socket Address structure
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = iProtocolPort;
	//SockAddr.sin_addr.s_addr = 



	// Connect the Socket
	if (connect(_Server, (PSOCKADDR)&SockAddr, sizeof(SockAddr))) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error conectando el socket.\n");
		WSACleanup();
		return FALSE;
	}


	// Cargo y codifico el archivo a mandar para saber el tamaño que ocupa en base64
	HANDLE Archivo = CreateFile(Path.c_str(), FILE_READ_DATA, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (Archivo == INVALID_HANDLE_VALUE) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error abriendo el archivo %s\n", Path.c_str());
		WSACleanup();
		return FALSE;
	}
	UINT TamArchivo = GetFileSize(Archivo, NULL);
	char *ArchivoChar = new char[TamArchivo];
	DWORD CaracteresLeidos = 0;
	if (ReadFile(Archivo, reinterpret_cast<LPVOID>(ArchivoChar), TamArchivo * sizeof(char), &CaracteresLeidos, NULL) == FALSE) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error leyendo el archivo %s.\n", Path.c_str());
		WSACleanup();
		return FALSE;
	}
	std::string &ArchivoStd = _base64_encode((unsigned char *)ArchivoChar, TamArchivo);
	delete[] ArchivoChar;
	size_t i = 0;

	Barra.Maximo(static_cast<float>(ArchivoStd.size()));
	Barra.Valor(0.0f);
	App.Eventos_Mirar();

	// Receive initial response from SMTP server
	_Recibir();
	// Send HELO server.com
	sprintf_s(szMsgLine, 255, "EHLO %s", ServidorSmtp);
	_Enviar(szMsgLine, TRUE);

	// AUTENTIFICAMOS
	_Enviar("AUTH LOGIN", TRUE);
	_Enviar(CorreoOrigen_Login, TRUE);			// login
	_Enviar(CorreoOrigen_Password, TRUE);		// pass

	// Send MAIL FROM: <sender@mydomain.com>
	sprintf_s(szMsgLine, 255, "MAIL FROM:<%s>", CorreoOrigen);
	_Enviar(szMsgLine, TRUE);

	// Send RCPT TO: <receiver@domain.com>
	sprintf_s(szMsgLine, 255, "RCPT TO:<%s>", CorreoDestino);
	_Enviar(szMsgLine, TRUE);

	// Send DATA
	_Enviar("DATA", TRUE);

	_Enviar("MIME-Version: 1.0");
	_Enviar("Content-Type:MULTIPART/MIXED;boundary=\"Boundary-=_StOHgENiGNeW\"\n"); 
	_Enviar("--Boundary-=_StOHgENiGNeW");
	_Enviar("Content-Type: text/plain"); 
	_Enviar("Content-Transfer-Encoding: 8bit\n");
	UINT VisualSudioVer = 2017;
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	sprintf_s(szMsgLine, 255, "RAVE : %f, SO : %s, Compilado con : VisualStudio %d\r\n", RAVE_VERSION, converterX.to_bytes(App.SO.c_str()).c_str(), VisualSudioVer);
	_Enviar(szMsgLine);

	_Enviar("--Boundary-=_StOHgENiGNeW");
	_Enviar("Content-Type:application/dump:\n name=\"Rave.dmp\"");
	_Enviar("Content-Transfer-Encoding: base64");
	_Enviar("Content-Disposition: attachment; filename=\"Rave.dmp\"\n");

	std::string Parte;
	// archivo codificado
	for (i = 0; i < ArchivoStd.size(); i += 997) {
		Parte = ArchivoStd.substr(i, 997);
		_EnviarNC(Parte.c_str());
		Barra.Valor(static_cast<float>(i));
		App.Eventos_Mirar();
	}

	_Enviar("--Boundary-=_StOHgENiGNeW");
	// Send blank line and a period
	_Enviar("\r\n.", TRUE);
	// Send QUIT
	_Enviar("QUIT", TRUE);

	// Close server socket and prepare to exit.
	closesocket(_Server);

	WSACleanup();

	if (_Error == TRUE) {
		Debug_Escribir_Varg(L"EnviarDump::Enviar Error durante la transmisión.\n");
		return FALSE;
	}
	Debug_Escribir_Varg(L"EnviarDump::Enviar Se ha enviado correctamente.\n");
	// Termino la aplicación
	PostQuitMessage(0);
	return TRUE;
}


const BOOL EnviarDump::Revisar(int iStatus, char *szFunction) {
	if ((iStatus != SOCKET_ERROR) && (iStatus))	return TRUE;	
	_Error = TRUE;
	return FALSE; // error
}



/*
Copyright (C) 2004-2008 René Nyffenegger

This source code is provided 'as-is', without any express or implied
warranty. In no event will the author be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this source code must not be misrepresented; you must not
claim that you wrote the original source code. If you use this source code
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original source code.

3. This notice may not be removed or altered from any source distribution.

René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/
std::string &EnviarDump::_base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)	ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)	char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)	ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))	ret += '=';
	}
	return ret;
}