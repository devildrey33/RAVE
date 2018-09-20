#pragma once

//#include "..\vlc-2.2.1\Include\vlc\vlc.h"

#include <vector>
#include "RaveBD.h"
//#include "TMedio.h"


#ifdef _WIN64
//	#ifdef _DEBUG
//		#pragma comment(lib, "..\\VLCx64-Debug\\libvlc.lib")	// LibVLC x64 Depuración
//	#else
		#pragma comment(lib, "..\\VLCx64\\libvlc.lib")	// LibVLC x64
//	#endif
#else 
	#pragma comment(lib, "..\\VLCx86\\libvlc.lib")	// LibVLC x86
#endif


// Clase para crear un array de chars con los argumentos que requiera el VLC, se ha diseñado de forma que resulte facil agregar nuevos argumentos
class ArgumentosVLC {
public:
	ArgumentosVLC(void) {
		Argumentos = NULL;
	};

	~ArgumentosVLC(void) {
		if (Argumentos != NULL) delete Argumentos;
		for (size_t i = 0; i < Args.size(); i++) {
			delete Args[i];
		}
	};

	// Lo ideal es pasar un char a esta funcion ya que al final el vlc mira los parametros como char
	// De todas formas se han implementado funciones para pasar los argumentos directamente con wchar_t y DWL::DWLString
	void AgregarArgumento(const char *Argumento) {
		char *Str = new char[512];
		strcpy_s(Str, 512, Argumento);
		Args.push_back(Str);
	};

	void AgregarArgumento(const wchar_t *Argumento) {
		char *Str = new char[512];
		int	TamRes = WideCharToMultiByte(CP_UTF8, NULL, Argumento, static_cast<int>(wcslen(Argumento)), Str, 512, NULL, NULL);
		Str[TamRes] = 0; // Termino el string, ya que en algunas compilaciones la funcion WideCharToMultiByte no pone el terminador.
		Args.push_back(Str);
	};

	/*void							AgregarArgumento(DWL::DWLString &Argumento) {
	char *Str = new char[512];
	int	TamRes = WideCharToMultiByte(CP_UTF8, NULL, Argumento(), Argumento.Tam(), Str, 512, NULL, NULL);
	Str[TamRes] = 0; // Termino el string, ya que en algunas compilaciones la funcion WideCharToMultiByte no pone el terminador.
	Args.push_back(Str);
	};*/

	const char **ObtenerArgumentos(void) {
		if (Argumentos != NULL) delete Argumentos;
		Argumentos = new const char *[Args.size()];
		for (size_t i = 0; i < Args.size(); i++) {
			Argumentos[i] = Args[i];
		}
		return Argumentos;
	};

	const char **operator () (void) {
		return ObtenerArgumentos();
	};
 
	const int TotalArgumentos(void) {
		return static_cast<int>(Args.size());
	}
private:
	const char				  **Argumentos;
	std::vector<char *>			Args;
};




class RaveVLC {
  public:
											RaveVLC(void);
										   ~RaveVLC(void);

	const BOOL								Iniciar(void);
	void									Terminar(void);

//	void CALLBACK							EventosVLC(const libvlc_event_t* event, void* ptr);

	const BOOL								AbrirMedio(BDMedio &Medio);
	void									CerrarMedio(void);

	const BOOL								Play(void);
	const BOOL								Pausa(void);
	const BOOL								Stop(void);

	const int								Volumen(void);
	void									Volumen(int nVolumen);

	Estados_Medio							ComprobarEstado(void);


	const float								TiempoActual(void);
	void									TiempoActual(float nTiempo);
	const UINT64							TiempoTotalMs(void);
	const UINT64							TiempoActualMs(void);

	void                                    Ratio(const float R);

	static void								TiempoStr(UINT64 TMS, wchar_t *StrTiempo);
	static void								TiempoStr(UINT64 TMS, std::wstring &StrTiempo);
	//	void									TiempoStrL(UINT64 TMS, wchar_t *StrTiempo);

	void									ActualizarIconos(int nIcono);
	
	BDMedio			                        MedioActual;

	UINT64                                  TiempoTotal;

//	void									RepintarVLC(void);
	void									AsignarPistaAudio(int nPista);

	static BOOL CALLBACK					EnumeracionVLC(HWND hWndWnd, LPARAM lParam);

	HWND									hWndVLC;

	static void								audio_prerendercb(void* data, unsigned char** buffer, size_t size);
	static void								audio_postrendercb(void* data, unsigned char* buffer, unsigned int channels, unsigned int rate, unsigned int nb_samples, unsigned int bits_per_sample, size_t size, int64_t pts);

	inline const BOOL                       Parseado(void) { return _Parseado; }
	void									ObtenerDatosParsing(void);
  protected:
	BOOL                                   _Parseado;
	//libvlc_media_t                        *_Media;
	libvlc_instance_t					  *_Instancia;
	libvlc_media_player_t				  *_MediaPlayer;
	libvlc_log_t						  *_Log;
	libvlc_event_manager_t				  *_Eventos;


};