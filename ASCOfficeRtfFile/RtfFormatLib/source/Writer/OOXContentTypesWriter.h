#pragma once


class OOXContentTypesWriter
{
public:
	void AddWriter()
	{
	}
	void AddContent( CString sType, CString sTarget )
	{
		for( int i = 0 ;i < (int)m_aTargets.size(); i++ )
			if( sTarget == m_aTargets[i] )
				return;
		m_aTargets.push_back( sTarget );
		m_aTypes.push_back( sType );
	}
	void AddExtension( CString sType, CString sTarget )
	{
		for( int i = 0 ;i < (int)m_aExtensions.size(); i++ )
			if( sTarget == m_aExtensions[i] )
				return;
		m_aExtensions.push_back( sTarget );
		m_aExtTypes.push_back( sType );
	}
	bool Save(CString sFolder)
	{
		HANDLE hFile = ::CreateFile(sFolder + _T("\\[Content_Types].xml"),
				GENERIC_WRITE,
				0,
				0,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				0);
		//ATLASSERT( INVALID_HANDLE_VALUE != hFile );

		if( INVALID_HANDLE_VALUE != hFile )
		{
			 DWORD dwBytesWritten;
			 CString sXml = CreateXml();
			 CStringA sXmlUTF = Convert::UnicodeToUtf8( sXml );
			 ::WriteFile( hFile, sXmlUTF, sXmlUTF.GetLength(), &dwBytesWritten, NULL );
			 CloseHandle( hFile );
			 return true;
		}
		return false;
	}

private: 
	std::vector< CString > m_aTargets;
	std::vector< CString > m_aTypes;

	std::vector< CString > m_aExtensions;
	std::vector< CString > m_aExtTypes;

	CString CreateXml()
	{
		CString sResult;
		sResult.Append( _T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>") );
		sResult.AppendChar('\n');
		sResult.Append( _T("<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">") );

		sResult.Append( _T("<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>") );
		sResult.Append( _T("<Default Extension=\"xml\" ContentType=\"application/xml\"/>") );

		for( int i = 0; i < (int)m_aExtensions.size(); i++ )
			sResult.AppendFormat( _T("<Default Extension=\"%ls\" ContentType=\"%ls\"/>"), m_aExtensions[i], m_aExtTypes[i]);

		for( int i = 0; i < (int)m_aTargets.size(); i++ )
			sResult.AppendFormat( _T("<Override PartName=\"%ls\" ContentType=\"%ls\"/>"), m_aTargets[i], m_aTypes[i]);
		sResult.Append( _T("</Types>") );
		return sResult;
	}
};