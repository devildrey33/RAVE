#include "stdafx.h"
#include "ExtensionesValidas.h"


ExtensionesValidas::ExtensionesValidas(void) {
}


ExtensionesValidas::~ExtensionesValidas(void) {
}

/* TODO : Ordenar extensiones por las mas utilizadas */
const Extension_Medio ExtensionesValidas::ObtenerExtension(const wchar_t *Extension) {
	if (_wcsicmp(Extension, TEXT("ASF")) == 0)			return Extension_ASF;
	if (_wcsicmp(Extension, TEXT("AMV")) == 0)			return Extension_AMV;
	if (_wcsicmp(Extension, TEXT("AVI")) == 0)			return Extension_AVI;
	if (_wcsicmp(Extension, TEXT("CDA")) == 0)			return Extension_CDA;
	if (_wcsicmp(Extension, TEXT("DIVX")) == 0)			return Extension_DIVX;
	if (_wcsicmp(Extension, TEXT("DV")) == 0)			return Extension_DV;
	if (_wcsicmp(Extension, TEXT("FLAC")) == 0)			return Extension_FLAC;
	if (_wcsicmp(Extension, TEXT("FLV")) == 0)			return Extension_FLV;
	if (_wcsicmp(Extension, TEXT("GXF")) == 0)			return Extension_GXF;
	if (_wcsicmp(Extension, TEXT("IT")) == 0)			return Extension_IT;
	if (_wcsicmp(Extension, TEXT("MKV")) == 0)			return Extension_MKV;
	if (_wcsicmp(Extension, TEXT("MID")) == 0)			return Extension_MID;
	if (_wcsicmp(Extension, TEXT("MOV")) == 0)			return Extension_MOV;
	if (_wcsicmp(Extension, TEXT("MPEG")) == 0)			return Extension_MPEG;
	if (_wcsicmp(Extension, TEXT("MPG")) == 0)			return Extension_MPG;
	if (_wcsicmp(Extension, TEXT("MPV")) == 0)			return Extension_MPV;
	if (_wcsicmp(Extension, TEXT("MPEG1")) == 0)		return Extension_MPEG1;
	if (_wcsicmp(Extension, TEXT("MPEG2")) == 0)		return Extension_MPEG2;
	if (_wcsicmp(Extension, TEXT("MPEG4")) == 0)		return Extension_MPEG4;
	if (_wcsicmp(Extension, TEXT("MPC")) == 0)			return Extension_MPC;
	if (_wcsicmp(Extension, TEXT("MPE")) == 0)			return Extension_MPE;
	if (_wcsicmp(Extension, TEXT("MP2")) == 0)			return Extension_MP2;
	if (_wcsicmp(Extension, TEXT("MP3")) == 0)			return Extension_MP3;
	if (_wcsicmp(Extension, TEXT("MP4")) == 0)			return Extension_MP4;
	if (_wcsicmp(Extension, TEXT("MP4V")) == 0)			return Extension_MP4V;
	if (_wcsicmp(Extension, TEXT("MTS")) == 0)			return Extension_MTS;
	if (_wcsicmp(Extension, TEXT("MXF")) == 0)			return Extension_MXF;
	if (_wcsicmp(Extension, TEXT("M1V")) == 0)			return Extension_M1V;
	if (_wcsicmp(Extension, TEXT("M2T")) == 0)			return Extension_M2T;
	if (_wcsicmp(Extension, TEXT("M2TS")) == 0)			return Extension_M2TS;
	if (_wcsicmp(Extension, TEXT("M2V")) == 0)			return Extension_M2V;
	if (_wcsicmp(Extension, TEXT("M4V")) == 0)			return Extension_M4V;
	if (_wcsicmp(Extension, TEXT("MOD")) == 0)			return Extension_MOD;
	if (_wcsicmp(Extension, TEXT("NSV")) == 0)			return Extension_NSV;
	if (_wcsicmp(Extension, TEXT("NUV")) == 0)			return Extension_NUV;
	if (_wcsicmp(Extension, TEXT("OGG")) == 0)			return Extension_OGG;
	if (_wcsicmp(Extension, TEXT("OGM")) == 0)			return Extension_OGM;
	if (_wcsicmp(Extension, TEXT("OGV")) == 0)			return Extension_OGV;
	if (_wcsicmp(Extension, TEXT("OGX")) == 0)			return Extension_OGX;
	if (_wcsicmp(Extension, TEXT("PS")) == 0)			return Extension_PS;
	if (_wcsicmp(Extension, TEXT("RM")) == 0)			return Extension_RM;
	if (_wcsicmp(Extension, TEXT("RMVB")) == 0)			return Extension_RMVB;
	if (_wcsicmp(Extension, TEXT("SDP")) == 0)			return Extension_SDP;
	if (_wcsicmp(Extension, TEXT("S3M")) == 0)			return Extension_S3M;
	if (_wcsicmp(Extension, TEXT("TOD")) == 0)			return Extension_TOD;
	if (_wcsicmp(Extension, TEXT("TS")) == 0)			return Extension_TS;
	if (_wcsicmp(Extension, TEXT("VOB")) == 0)			return Extension_VOB;
	if (_wcsicmp(Extension, TEXT("VRO")) == 0)			return Extension_VRO;
	if (_wcsicmp(Extension, TEXT("WAV")) == 0)			return Extension_WAV;
	if (_wcsicmp(Extension, TEXT("WEBM")) == 0)			return Extension_WEBM;
	if (_wcsicmp(Extension, TEXT("WMA")) == 0)			return Extension_WMA;
	if (_wcsicmp(Extension, TEXT("WMV")) == 0)			return Extension_WMV;
	if (_wcsicmp(Extension, TEXT("XM")) == 0)			return Extension_XM;

	if (_wcsicmp(Extension, TEXT("CRDOWNLOAD")) == 0)	return Extension_CRDOWNLOAD;
	if (_wcsicmp(Extension, TEXT("OPDOWNLOAD")) == 0)	return Extension_OPDOWNLOAD;
	if (_wcsicmp(Extension, TEXT("M3U")) == 0)			return Extension_M3U;
	if (_wcsicmp(Extension, TEXT("M3U8")) == 0)			return Extension_M3U8;
	if (_wcsicmp(Extension, TEXT("SRT")) == 0)			return Extension_SRT;
		

	return Extension_NOSOPORTADA;
}

// Obtiene el tipo de medio esperado... (despues puede ser otra cosa...)
const Tipo_Medio ExtensionesValidas::ObtenerTipoMedio(const Extension_Medio Extension) {
	switch (Extension) {
		case Extension_ASF:		case Extension_AMV:		case Extension_AVI:		case Extension_DIVX:	case Extension_DV:		case Extension_FLV:		case Extension_GXF:		case Extension_IT:	
		case Extension_MKV:		case Extension_MOV:		case Extension_MPEG:	case Extension_MPG:		case Extension_MPV:		case Extension_MPEG1:	case Extension_MPEG2:	case Extension_MPEG4:
		case Extension_MPE:		case Extension_MP4:		case Extension_MP4V:	case Extension_MTS:		case Extension_MXF:		case Extension_M1V:		case Extension_M2T:		case Extension_M2TS:
		case Extension_M2V:		case Extension_M4V:		case Extension_NSV:		case Extension_NUV:		case Extension_OGM:		case Extension_OGV:
		case Extension_OGX:		case Extension_PS:		case Extension_RM:		case Extension_RMVB:	case Extension_SDP:		case Extension_TOD:		case Extension_TS:		case Extension_VOB:
		case Extension_VRO:		case Extension_WMV:		case Extension_CRDOWNLOAD:						case Extension_OPDOWNLOAD:
			return Tipo_Medio_Video;
		// El Tipo_Medio_IpTv solo se puede obtener una vez cargado el M3u desde una URL
		case Extension_FLAC:	case Extension_MID:		case Extension_MP2:		case Extension_MP3:		case Extension_S3M:		case Extension_WAV:		case Extension_WEBM:	case Extension_WMA:	
		case Extension_XM:		case Extension_OGG:		case Extension_MOD:		case Extension_MPC:
			return Tipo_Medio_Audio;
		case Extension_CDA:
			return Tipo_Medio_CDAudio;
		case Extension_M3U: case Extension_M3U8 :
			return Tipo_Medio_Lista;
		case Extension_SRT:
			return Tipo_Medio_Subtitulos;
	}
	return Tipo_Medio_INDEFINIDO;
}


// Actualizar tambien en en instalador HKEY_LOCAL_MACHINE "Software\Clients\Media\RAVE\Capabilities\FileAssociations" -> ".EXTENSION" : "RAVE.Audio/Video/Lista/CDAudio"
const wchar_t *ExtensionesValidas::ExtensionStr(const Extension_Medio Extension) {
	switch (Extension) {
		case Extension_ASF			:	return L"ASF";
		case Extension_AMV			:	return L"AMV";
		case Extension_AVI			:	return L"AVI";
		case Extension_CDA			:	return L"CDA";
		case Extension_DIVX			:	return L"DIVX";
		case Extension_DV			:	return L"DV";
		case Extension_FLAC			:	return L"FLAC";
		case Extension_FLV			:	return L"FLV";
		case Extension_GXF			:	return L"GXF";
		case Extension_IT			:	return L"IT";
		case Extension_MID			:	return L"MID";
		case Extension_MKV			:	return L"MKV";
		case Extension_MOV			:	return L"MOV";
		case Extension_MPEG			:	return L"MPEG";
		case Extension_MPG			:	return L"MPG";
		case Extension_MPV			:	return L"MPV";
		case Extension_MPEG1		:	return L"MPEG1";
		case Extension_MPEG2		:	return L"MPEG2";
		case Extension_MPEG4		:	return L"MPEG4";
		case Extension_MPC			:	return L"MPC";
		case Extension_MPE			:	return L"MPE";
		case Extension_MP2			:	return L"MP2";
		case Extension_MP3			:	return L"MP3";
		case Extension_MP4			:	return L"MP4";
		case Extension_MP4V			:	return L"MP4V";
		case Extension_MTS			:	return L"MTS";
		case Extension_MXF			:	return L"MXF";
		case Extension_M1V			:	return L"M1V";
		case Extension_M2T			:	return L"M2T";
		case Extension_M2TS			:	return L"M2TS";
		case Extension_M2V			:	return L"M2V";
		case Extension_M3U			:	return L"M3U";
		case Extension_M3U8			:	return L"M3U8";
		case Extension_M4V			:	return L"M4V";
		case Extension_MOD			:	return L"MOD";
		case Extension_NSV			:	return L"NSV";
		case Extension_NUV			:	return L"NUV";
		case Extension_OGG			:	return L"OGG";
		case Extension_OGM			:	return L"OGM";
		case Extension_OGV			:	return L"OGV";
		case Extension_OGX			:	return L"OGX";
		case Extension_PS			:	return L"PS";
		case Extension_RM			:	return L"RM";
		case Extension_RMVB			:	return L"RMVB";
		case Extension_SDP			:	return L"SDP";
		case Extension_S3M			:	return L"S3M";
		case Extension_TOD			:	return L"TOD";
		case Extension_TS			:	return L"TS";
		case Extension_VOB			:	return L"VOB";
		case Extension_VRO			:	return L"VRO";
		case Extension_WAV			:	return L"WAV";
		case Extension_WEBM			:	return L"WEBM";
		case Extension_WMA			:	return L"WMA";
		case Extension_WMV			:	return L"WMV";
		case Extension_XM			:	return L"XM";

		case Extension_CRDOWNLOAD	:	return L"CRDOWNLOAD";
		case Extension_OPDOWNLOAD	:	return L"OPDOWNLOAD";
		case Extension_SRT			:	return L"SRT";
	}
	return L"";
}