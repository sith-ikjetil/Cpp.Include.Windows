///////////////////////////////////////////////////////////////////
//: Title        : itsoftware-win.cpp
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows.
#include <Windows.h>
#include "itsoftware-win.h"

//
// using namespace
//
using namespace ItSoftware;
using namespace ItSoftware::Win;

namespace ItSoftware
{
	namespace Win
	{
		HHOOK ItsWin::hCBTHook = NULL;
	}
}