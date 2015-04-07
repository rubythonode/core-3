#ifndef DOCUMENT_RELS_WRITER
#define DOCUMENT_RELS_WRITER

#include "../../XlsxSerializerCom/Common/Common.h"

namespace Writers
{
	class DocumentRelsWriter
	{
		CString	m_sDir;
	public:
		DocumentRelsWriter(CString sDir):m_sDir(sDir)
		{
		}
		void Write()
		{
			CString s_Common;

			s_Common = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?> \
<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"> \
<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"word/document.xml\"/> \
<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/> \
<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/> \
</Relationships>");

            OOX::CPath fileName = m_sDir + FILE_SEPARATOR_STR + _T("_rels") + FILE_SEPARATOR_STR + _T(".rels");

            CFile oFile;
            oFile.CreateFile(fileName.GetPath());
            oFile.WriteStringUTF8(s_Common);
			oFile.CloseFile();
		}
	};
}
#endif	// #ifndef DOCUMENT_RELS_WRITER