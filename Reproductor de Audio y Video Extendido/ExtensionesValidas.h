#pragma once

//#include "TDisco.h"

enum Ubicacion_Medio {
	Ubicacion_Medio_INDEFINIDO  = -1,
	Ubicacion_Medio_Local		=  0,
	Ubicacion_Medio_Red			=  1,
	Ubicacion_Medio_Internet	=  2
};

enum Tipo_Medio {
	Tipo_Medio_INDEFINIDO	= -1,
	Tipo_Medio_Audio		=  1,
	Tipo_Medio_Video		=  2,
	Tipo_Medio_CDAudio		=  3,
	Tipo_Medio_Lista		=  4,
	Tipo_Medio_Subtitulos	=  5,
	Tipo_Medio_IpTv			=  6
};


enum Extension_Medio {
	Extension_NOSOPORTADA = 0,
	Extension_ASF,
	Extension_AMV,
	Extension_AVI,
	Extension_CDA,
	Extension_DIVX,
	Extension_DV,
	Extension_FLAC,
	Extension_FLV,
	Extension_GXF,
	Extension_IT,
	Extension_MKV,
	Extension_MID,
	Extension_MOV,
	Extension_MPEG,
	Extension_MPG,
	Extension_MPV,
	Extension_MPEG1,
	Extension_MPEG2,
	Extension_MPEG4,
	Extension_MPC,
	Extension_MPE,
	Extension_MP2,
	Extension_MP3,
	Extension_MP4,
	Extension_MP4V,
	Extension_MTS,
	Extension_MXF,
	Extension_M1V,
	Extension_M2T,
	Extension_M2TS,
	Extension_M2V,
	Extension_M3U,
	Extension_M3U8,
	Extension_M4V,
	Extension_M4A,
	Extension_MOD,
	Extension_NSV,
	Extension_NUV,
	Extension_OGG,
	Extension_OGM,
	Extension_OGV,
	Extension_OGX,
	Extension_PS,
	Extension_RM,
	Extension_RMVB,
	Extension_SDP,
	Extension_S3M,
	Extension_TOD,
	Extension_TS,
	Extension_VOB,
	Extension_VRO,
	Extension_WAV,
	Extension_WEBM,
	Extension_WMA,
	Extension_WMV,
	Extension_XM,
	
	Extension_CRDOWNLOAD,
	Extension_OPDOWNLOAD,
	Extension_SRT,
	// Ultima extensi�n
	Extension_FINAL
};

class ExtensionesValidas {
  public:
									ExtensionesValidas(void);
								   ~ExtensionesValidas(void);
	static const Extension_Medio	ObtenerExtension(const wchar_t *Extension);
	static const Tipo_Medio			ObtenerTipoMedio(const Extension_Medio Extension);
	static const wchar_t           *ExtensionStr(const Extension_Medio Extension);
};