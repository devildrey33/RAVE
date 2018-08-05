#include "stdafx.h"
#include "AsociarReproductor.h"
#include "RAVE_Configuracion.h"
#include "DRegistro.h"
#include <Shlobj.h>
#include "DStringUtils.h"

using namespace DWL;

AsociarReproductor::AsociarReproductor() {
}


AsociarReproductor::~AsociarReproductor() {
}


void AsociarReproductor::RegistrarApp(void) {

	std::wstring TmpTxt, TmpTxt2;
	TCHAR Res[1024];
	TCHAR Backup[1024];
	// Información para la des-instalación
/*	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"DisplayName", L"RAVE");
	TmpTxt = DString_ToStrF(RAVE_VERSION);//	TmpTxt.sprintf(L"%03f", APP_NUM_VER);
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"DisplayVersion", TmpTxt());
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"InstallLocation", Sistema.App.AppPath());
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"Publisher", L"devildrey33");
	TmpTxt.sprintf(L"%sDesInstaladorRAVE.exe -EliminarRAVE", Sistema.App.AppPath()); // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\RAVE
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"UninstallString", TmpTxt());
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"UninstallPath", TmpTxt());
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"URLInfoAbout", L"http://rave.devildrey33.es");
	TmpTxt.sprintf(L"%sRAVE.exe,0", Sistema.App.AppPath());
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE", L"DisplayIcon", TmpTxt());*/

	// Parte que registra la aplicación para que salga en Panel de control -> Programas predeterminados.
	// HKLM\Software\RegisteredApplications
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\RegisteredApplications", L"RAVE", L"Software\\Clients\\Media\\RAVE\\Capabilities");
	// HKLM\Software\Clients\Media\RAVE
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE", L"", L"RAVE, Reproductor de Audio y Video Extendido.");
	// HKLM\Software\Clients\Media\RAVE\InstallInfo -> [SZ]HideIconsCommand
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -EsconderIconos";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\InstallInfo", L"HideIconsCommand", TmpTxt.c_str());
	// HKLM\Software\Clients\Media\RAVE\InstallInfo -> [SZ]ReinstallCommand
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -InstalarRegistroWindows";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\InstallInfo", L"ReinstallCommand", TmpTxt.c_str());
	// HKLM\Software\Clients\Media\RAVE\InstallInfo -> [SZ]ShowIconsCommand
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -MostrarIconos";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\InstallInfo", L"ShowIconsCommand", TmpTxt.c_str());
	// HKLM\Software\Clients\Media\RAVE\InstallInfo -> [DWORD]IconsVisible
	DRegistro::AsignarValor_DWORD(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\InstallInfo", L"IconsVisible", 1);

	// HKLM\Software\Clients\Media\RAVE\Capabilities -> ApplicationDescription
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\Capabilities", L"ApplicationDescription", L"Reproductor de Audio y Video Extendido basado en VLC.");
	// HKLM\Software\Clients\Media\RAVE\Capabilities -> ApplicationName
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\Capabilities", L"ApplicationName", L"RAVE, Reproductor de Audio y Video Extendido.");
	// HKLM\Software\Clients\Media\RAVE\Capabilities\FileAssociations
	for (size_t i = Extension_ASF; i < Extension_FINAL; i++) {
		TmpTxt = L"."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
		switch (ExtensionesValidas::ObtenerTipoMedio(static_cast<Extension_Medio>(i))) {
			case Tipo_Medio_Audio	: TmpTxt2 = L"RAVE.Audio";		break;
			case Tipo_Medio_Video	: TmpTxt2 = L"RAVE.Video";		break;
			case Tipo_Medio_CDAudio	: TmpTxt2 = L"RAVE.CDAudio";	break;
			case Tipo_Medio_Lista	: TmpTxt2 = L"RAVE.Lista";		break;
		}
		DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\Capabilities\\FileAssociations", TmpTxt.c_str(), TmpTxt2.c_str());
	}

	// Parte que registra los tipos basicos de la aplicación : Audio, Video, CDAudio, Lista
	// Se hace en HKEY_LOCAL_MACHINE/Software/Clases y en HKEY_CLASSES_ROOT porque en windows 7 no son la misma clave....

	// Tipo_Medio_Audio
	// - Descripción
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio", L"", L"Archivo de Audio");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio", L"", L"Archivo de Audio");
	// - Icono 53
	TmpTxt = App.AppPath + L"RAVE.exe,53";	
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\DefaultIcon", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\DefaultIcon", L"", TmpTxt.c_str());
	// - Shell
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell", L"", L"Reproducir");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell", L"", L"Reproducir");
	// - Shell -> Añadir a lista
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	// - Shell -> Open
//	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Open\\command"), L""), TmpTxt());
//	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Open\\command"), L""), TmpTxt());
	// - Shell -> Reproducir
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -r \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());

	// Tipo_Medio_Video
	// - Descripción
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video", L"", L"Archivo de Video");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video", L"", L"Archivo de Video");
	// - Icono 52
	TmpTxt = App.AppPath + L"RAVE.exe,52";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\DefaultIcon", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\DefaultIcon", L"", TmpTxt.c_str());
	// - Shell
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell", L"", L"Reproducir");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell", L"", L"Reproducir");
	// - Shell -> Añadir a lista
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	// - Shell -> Open
//	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Open\\command"), L""), TmpTxt());
//	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Open\\command"), L""), TmpTxt());
	// - Shell -> Reproducir
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -r \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());
	
	// Tipo_Medio_Lista
	// - Descripción
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista", L"", L"Lista de medios");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista", L"", L"Lista de medios");
	// - Icono 41
	TmpTxt = App.AppPath + L"RAVE.exe,41";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\DefaultIcon", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\DefaultIcon", L"", TmpTxt.c_str());
	// - Shell
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell", L"", L"Reproducir");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell", L"", L"Reproducir");
	// - Shell -> Añadir a lista
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	// - Shell -> Open
//	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, "Software\\Classes\\RAVE.Audio\\Shell\\Open\\command"), ""), TmpTxt());
//	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, "RAVE.Audio\\Shell\\Open\\command"), ""), TmpTxt());
	// - Shell -> Reproducir
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -r \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());

	// Tipo_Medio_CDAudio
	// - Descripción
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio", L"", L"Pista de Audio");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio", L"", L"Pista de Audio");
	// - Icono 76
	TmpTxt = App.AppPath + L"RAVE.exe,76";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\DefaultIcon", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\DefaultIcon", L"", TmpTxt.c_str());
	// - Shell
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell", L"", L"Reproducir");
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell", L"", L"Reproducir");
	// - Shell -> Añadir a lista
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Añadir a lista\\command", L"", TmpTxt.c_str());
	// - Shell -> Open
//	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, "Software\\Classes\\RAVE.Audio\\Shell\\Open\\command"), ""), TmpTxt());
//	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, "RAVE.Audio\\Shell\\Open\\command"), ""), TmpTxt());
	// - Shell -> Reproducir
	TmpTxt = L"\"" + App.AppPath + L"RAVE.exe\" -r \"%1\"";
	DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());
	DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Reproducir\\command", L"", TmpTxt.c_str());

	// Parte que enlaza las extensiones con los tipos de medio (Se guardara un backup para poder desinstalar)
	for (size_t i = Extension_ASF; i < Extension_FINAL; i++) {
		//for (size_t i = 0; i < Sistema.App.Tipos.Tipos.size(); i++) {
		// Creo la copia de seguridad si no existe.
		ZeroMemory(Backup, sizeof(TCHAR) *1024);
		TmpTxt = L"Software\\Classes\\."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
//		TmpTxt.sprintf("Software\\Classes\\.%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, TmpTxt.c_str(), L"", Backup, 1024);			
		if (Backup[0] == 0) { // No hay backup, lo creamos
			ZeroMemory(Res, sizeof(TCHAR) *1024);
			DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, TmpTxt.c_str(), L"", Res, 1024);			
			DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, TmpTxt.c_str(), L"RAVE.Backup", Res);
		}

		ZeroMemory(Backup, sizeof(TCHAR) *1024);
//		TmpTxt.sprintf(".%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		TmpTxt = L"."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
		DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, TmpTxt.c_str(), L"", Backup, 1024);			
		if (Backup[0] == 0) { // No hay backup, lo creamos
			DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, TmpTxt.c_str(), L"", Res, 1024);			
			DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, TmpTxt.c_str(), L"RAVE.Backup", Res);
		}

		// Guardo la clase asociada
		switch (ExtensionesValidas::ObtenerTipoMedio(static_cast<Extension_Medio>(i))) {
			case Tipo_Medio_Audio	:	TmpTxt2 = L"RAVE.Audio";		break;
			case Tipo_Medio_Video	:	TmpTxt2 = L"RAVE.Video";		break;
			case Tipo_Medio_CDAudio :	TmpTxt2 = L"RAVE.CDAudio";		break;
			case Tipo_Medio_Lista	:	TmpTxt2 = L"RAVE.Lista";		break;
		}
		TmpTxt = L"Software\\Classes\\."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
//		TmpTxt.sprintf("Software\\Classes\\.%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, TmpTxt.c_str(), L"", TmpTxt2.c_str());

//		TmpTxt.sprintf(".%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		TmpTxt = L"."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
		DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, TmpTxt.c_str(), L"", TmpTxt2.c_str());

		// Activo la clase para windows vista / 7
		TmpTxt = L"Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\."; TmpTxt += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i)); TmpTxt += L"\\UserChoice";
//		TmpTxt.sprintf("Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\.%s\\UserChoice"), Sistema.App.Tipos.Tipos[i]->Extension());
		DRegistro::AsignarValor_String(HKEY_CURRENT_USER, TmpTxt.c_str(), L"Progid", TmpTxt2.c_str());
	}

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}



void AsociarReproductor::DesRegistrarApp(void) {
	std::wstring Clave;
	std::wstring Comp;
	TCHAR Backup[1024];

	// Necesito restaurar todos los tipos por defecto (por si se ha borrado alguno despues)
//	Sistema.App.Tipos.TiposPorDefecto();

	// Informacion del Uninstall de windows
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"DisplayName");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"DisplayVersion");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"InstallLocation");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"Publisher");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"UninstallString");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"UninstallPath");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"URLInfoAbout");
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RAVE",	L"DisplayIcon");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",		L"RAVE");

	// HKLM\Software\RegisteredApplications
	DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, L"Software\\RegisteredApplications",								L"RAVE");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\",								L"InstallInfo");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\Capabilities\\",					L"FileAssociations");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\RAVE\\",								L"Capabilities");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Clients\\Media\\",										L"RAVE");

	// Borro las clases basicas de medios
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Añadir a lista\\",			L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\",							L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\open\\",					L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\",							L"open");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Reproducir\\",				L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\",							L"Reproducir");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\",								L"Shell");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\",								L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes", L"RAVE.Audio");

	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Añadir a lista\\",			L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\",							L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\open\\",					L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\",							L"open");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Reproducir\\",				L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\",							L"Reproducir");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\",								L"Shell");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\",								L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes",												L"RAVE.Video");

	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Añadir a lista\\",		L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\",						L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\open\\",					L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\",						L"open");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Reproducir\\",			L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\",						L"Reproducir");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\",								L"Shell");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\",								L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes",												L"RAVE.CDAudio");

	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Añadir a lista\\",			L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\",							L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\open\\",					L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\",							L"open");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Reproducir\\",				L"command");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\",							L"Reproducir");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\",								L"Shell");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\",								L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_LOCAL_MACHINE, L"Software\\Classes",												L"RAVE.Lista");

	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Añadir a lista\\",								L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\",												L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\open\\",										L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\",												L"open");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Reproducir\\",									L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\",												L"Reproducir");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\",													L"Shell");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Audio\\",													L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"",																L"RAVE.Audio");

	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Añadir a lista\\",								L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\",												L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\open\\",										L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\",												L"open");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Reproducir\\",									L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\",												L"Reproducir");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\",													L"Shell");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Video\\",													L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"",																L"RAVE.Video");

	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Añadir a lista\\",							L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\",											L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\open\\",										L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\",											L"open");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Reproducir\\",								L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\",											L"Reproducir");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\",													L"Shell");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\",													L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"",																L"RAVE.CDAudio");

	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Añadir a lista\\",								L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\",												L"Añadir a lista");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\open\\",										L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\",												L"open");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Reproducir\\",									L"command");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\",												L"Reproducir");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\",													L"Shell");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"RAVE.Lista\\",													L"DefaultIcon");
	DRegistro::EliminarClave(HKEY_CLASSES_ROOT, L"",																L"RAVE.Lista");

	// Restauro copias de seguridad de las extensiones
	for (size_t i = Extension_ASF; i < Extension_FINAL; i++) {
		ZeroMemory(Backup, sizeof(TCHAR) * 1024);
		Clave = L"Software\\Classes\\."; Clave += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
//		Clave.sprintf(TEXT("Software\\Classes\\.%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, Clave.c_str(), L"RAVE.Backup", Backup, 1024);
		if (Backup[0] != 0) {
			DRegistro::AsignarValor_String(HKEY_LOCAL_MACHINE, Clave.c_str(), L"", Backup);
		}
		DRegistro::EliminarValor(HKEY_LOCAL_MACHINE, Clave.c_str(), L"RAVE.Backup");
		ZeroMemory(Backup, sizeof(TCHAR) * 1024);
		Clave = L"\\."; Clave += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
//		Clave.sprintf(TEXT("\\.%s"), Sistema.App.Tipos.Tipos[i]->Extension());
		DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, Clave.c_str(), L"RAVE.Backup", Backup, 1024);
		if (Backup[0] != 0) {
			DRegistro::AsignarValor_String(HKEY_CLASSES_ROOT, Clave.c_str(), L"", Backup);
		}
		DRegistro::EliminarValor(HKEY_CLASSES_ROOT, Clave.c_str(), L"RAVE.Backup");
		// Miro la selección de la asociación (win vista / 7)
		ZeroMemory(Backup, sizeof(TCHAR) * 1024);
//		Clave.sprintf(TEXT("Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\.%s\\UserChoice"), Sistema.App.Tipos.Tipos[i]->Extension());
		Clave = L"Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\."; Clave += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i)); Clave += L"\\UserChoice";
		DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, Clave.c_str(), L"Progid", Backup, 1024);
		Comp = Backup;
		if (Comp == L"RAVE.Audio" || Comp == L"RAVE.Video" || Comp == L"RAVE.CDAudio" || Comp == L"RAVE.Lista") {
			Clave = L"Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\."; Clave += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
//			Clave.sprintf(TEXT("Software\\Microsoft\\Windows\\Current version\\Explorer\\FileExts\\.%s"), Sistema.App.Tipos.Tipos[i]->Extension());
			DRegistro::EliminarClave(HKEY_CURRENT_USER, Clave.c_str(), L"UserChoice");

		}

	}

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}





const BOOL AsociarReproductor::ComprobarAsociaciones(void) {
	std::wstring Clave, Nombre, Str;
	// Comprobamos si se ha re-asignado alguna extension
	for (size_t i = Extension_ASF; i < Extension_FINAL; i++) {
		switch (ExtensionesValidas::ObtenerTipoMedio(static_cast<Extension_Medio>(i))) {
			case Tipo_Medio_Audio	:	Nombre = L"RAVE.Audio";		break;
			case Tipo_Medio_Video	:	Nombre = L"RAVE.Video";		break;
			case Tipo_Medio_CDAudio	:	Nombre = L"RAVE.CDAudio";	break;
			case Tipo_Medio_Lista	:	Nombre = L"RAVE.Lista";		break;
		}
		Clave = L"Software\\Classes\\."; Clave += ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
		DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, Clave.c_str(), L"", Str);
		if (Nombre != Str)	return FALSE;
		Clave = L'.' + ExtensionesValidas::ExtensionStr(static_cast<Extension_Medio>(i));
		DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, Clave.c_str(), L"", Str);
		if (Nombre != Str)	return FALSE;
	}

	// Comprobamos que las clases esten correctas (Audio, Video, CDAudio, Lista)
	Nombre = L"\"" + App.AppPath + L"RAVE.exe\" \"%1\"";	
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Añadir a lista\\command", L"", Str);
	if (Nombre != Str)		return FALSE;

	Nombre = L"\"" + App.AppPath + L"RAVE.exe\" -r \"%1\"";
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Audio\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Audio\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Video\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Video\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.CDAudio\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.CDAudio\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_LOCAL_MACHINE, L"Software\\Classes\\RAVE.Lista\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;
	DRegistro::ObtenerValor_String(HKEY_CLASSES_ROOT, L"RAVE.Lista\\Shell\\Reproducir\\command", L"", Str);
	if (Nombre != Str)		return FALSE;

	return TRUE;
}
