#ifndef DLISTAEX_SUBITEM_H
#define DLISTAEX_SUBITEM_H

namespace DWL {
	class DListaEx_SubItem	{
	  public:
						DListaEx_SubItem() { };
						DListaEx_SubItem(const TCHAR *nTexto)  { Texto = nTexto; };
//						DListaEx_SubItem(std::wstring &nTexto) { Texto = nTexto; };
		               ~DListaEx_SubItem() { };
		std::wstring	Texto;
	};
}
#endif