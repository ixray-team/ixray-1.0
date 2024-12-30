#pragma once

#ifdef XRGAME_EXPORTS
	#include "../../../xrCore/XMLParser/xrXMLParser.h"
#else
	#include "../xrCore/XMLParser/xrXMLParser.h"
#endif

class CUIXml :public CXml
{

protected:
	virtual shared_str correct_file_name	(LPCSTR path, LPCSTR fn);

};