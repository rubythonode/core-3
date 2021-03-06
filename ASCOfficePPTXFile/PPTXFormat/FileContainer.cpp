﻿/*
 * (c) Copyright Ascensio System SIA 2010-2017
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at Lubanas st. 125a-25, Riga, Latvia,
 * EU, LV-1021.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */


#include "FileContainer.h"
#include "FileTypes.h"

#include "LegacyDiagramText.h"
#include "FileFactory.h"
#include "WrapperFile.h"

#include "../../Common/DocxFormat/Source/DocxFormat/Rels.h"
#include "../../Common/DocxFormat/Source/DocxFormat/ContentTypes.h"
#include "../../Common/DocxFormat/Source/DocxFormat/External/HyperLink.h"
#include "../../Common/DocxFormat/Source/DocxFormat/FileTypes.h"
#include "../../DesktopEditor/common/Directory.h"

#include <map>

namespace PPTX
{
	static std::wstring arDefDirectories [9][2] = //in ppt Directory
	{
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide",			L"slides"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideLayout",	L"slideLayouts"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/slideMaster",	L"slideMasters"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesSlide",		L"notesSlides"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesMaster",	L"notesMasters"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/handoutMaster",	L"handoutMasters"},		
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/comments",		L"comments"},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/commentAuthors",	L""},
		{L"http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme",			L"theme"},
	};

	static std::wstring FindFileInDirectory(std::wstring directory, std::wstring filename)
	{
		if (directory.empty()) return L"";

		if (directory[directory.length() - 1] == FILE_SEPARATOR_CHAR)
			directory = directory.substr(0, directory.length() - 1);

		int pos_ppt = directory.rfind(L"ppt");
		if (pos_ppt >= 0)
		{
			directory = directory.substr(0, pos_ppt - 1); //root directory
		}
        std::vector<std::wstring> arrFiles = NSDirectory::GetFiles(directory, true);

        for (size_t i = 0 ; i < arrFiles.size(); i++)
		{
			if (std::wstring::npos != arrFiles[i].find(filename))
			{
				return arrFiles[i];
			}
		}
		return L"";
	}

	void FileContainer::read(const OOX::CPath& filename)
	{
		//not implement FileContainer.read
	}

	void FileContainer::read(const OOX::CRels& rels, const OOX::CPath& path)
	{
		//not implement FileContainer.read
	}
	smart_ptr<PPTX::LegacyDiagramText> FileContainer::legacyDiagramText(const OOX::RId& rId) const 
	{
        std::map<std::wstring, smart_ptr<OOX::File>>::const_iterator pPair = m_mContainer.find(rId.get());
        if (pPair == m_mContainer.end ())
            return smart_ptr<LegacyDiagramText>();
        return pPair->second.smart_dynamic_cast<LegacyDiagramText>();
	}
	void FileContainer::read(const OOX::CPath& filename, FileMap& map, IPPTXEvent* Event)
	{
		OOX::CRels rels(filename);
		OOX::CPath path = filename.GetDirectory();
		read(rels, path, map, Event);
	}

	OOX::CPath FileContainer::CorrectPathRels(const OOX::CPath& path, OOX::Rels::CRelationShip* relation )
	{
        if (relation->IsExternal()) return relation->Target();

		OOX::CPath filename = path / relation->Target();
		
		if ( NSFile::CFileBinary::Exists(filename.GetPath()) == true ) return filename;
		
		//file_1_ (1).pptx			
		std::wstring strDefDirectory;
		for (int i = 0; i < 9; i++)
		{
			if (relation->Type() == arDefDirectories[i][0])
			{
				strDefDirectory = arDefDirectories[i][1];
				break;
			}
		}
		
		OOX::CPath new_filename = strDefDirectory + FILE_SEPARATOR_STR + relation->Filename().GetFilename();
		
		filename = path / new_filename;
		
		if (NSFile::CFileBinary::Exists(filename.GetPath()) == false) 
		{
			filename = FindFileInDirectory(path.GetPath(), relation->Filename().GetFilename()); // find true path by filename

			if (NSFile::CFileBinary::Exists(filename.GetPath()) == false) 
				return filename;
		}

		*relation = OOX::Rels::CRelationShip( relation->rId(), relation->Type(), filename);

		return filename;
	}
	void FileContainer::read(const OOX::CRels& rels, const OOX::CPath& path, FileMap& map, IPPTXEvent* Event)
	{
		bool bIsSlide = false;
		OOX::File* pSrcFile = dynamic_cast<OOX::File*>(this);
		if (NULL != pSrcFile)
			bIsSlide = (pSrcFile->type() == OOX::Presentation::FileTypes::Slide) ? true : false;

		size_t nCount = rels.m_arrRelations.size();

        for (size_t i = 0; i < nCount; ++i)
		{
			OOX::Rels::CRelationShip* pRelation = rels.m_arrRelations[i];

 			OOX::CPath normPath = CorrectPathRels(path, pRelation);

			std::map<std::wstring, smart_ptr<OOX::File>>::const_iterator pPair = map.find(normPath);

            if (bIsSlide && (pRelation->Type() == OOX::FileTypes::HyperLink ||
                             pRelation->Type() == OOX::Presentation::FileTypes::Slide))
			{// + external audio, video ...

                smart_ptr<OOX::File> file;

                file = smart_ptr<OOX::File>(new OOX::HyperLink(pRelation->Target()));

                normPath = pRelation->Target();
                Add(pRelation->rId(), file);

			}
			else
			{
                if (pPair != map.m_map.end())
				{
                    Add(pRelation->rId(), pPair->second);
				}
				else
				{
					long percent = Event->GetPercent();

					smart_ptr<OOX::File> file = PPTX::FileFactory::CreateFilePPTX(normPath, *pRelation, map);

					if (file.IsInit() == false)
						continue;
					
					map.add(normPath, file);
					Add(pRelation->rId(), file);

					smart_ptr<FileContainer> pContainer = file.smart_dynamic_cast<FileContainer>();
                    Event->Progress(0, percent + m_lPercent);

                    if (pContainer.IsInit())
					{
						pContainer->m_lPercent = m_lPercent;
						Event->AddPercent(m_lPercent);

						pContainer->read(normPath, map, Event);
						m_bCancelled = pContainer->m_bCancelled;
					}
                    //todo детально разобраться и вернуть проверку res.(до перехода на cross platform все было хорошо)
                    //на презентация с hyperlink выходим при достижении 100%. Проценты считаются от количества обработанных файлов, а hyperlink не файл(Ligninger_og_uligheder.pptx)
                    if (m_bCancelled)
                    {
                       break;
                    }
                }
			}
		}
	}

	void FileContainer::write(const OOX::CPath& filename, const OOX::CPath& directory, OOX::CContentTypes& content) const
	{
		OOX::CRels rels;
		OOX::CPath current = filename.GetDirectory();
		write(rels, current, directory, content);
		rels.Write(filename);
	}

	void FileContainer::write(OOX::CRels& rels, const OOX::CPath& curdir, const OOX::CPath& directory, OOX::CContentTypes& content) const
	{
		std::map<std::wstring, size_t> mNamePair;
		for (std::map<std::wstring, smart_ptr<OOX::File>>::const_iterator pPair = m_mContainer.begin(); pPair != m_mContainer.end(); ++pPair)
		{
			smart_ptr<OOX::File>     pFile = pPair->second;
			smart_ptr<OOX::External> pExt  = pFile.smart_dynamic_cast<OOX::External>();
			if ( !pExt.IsInit() )
			{
				smart_ptr<PPTX::WrapperFile> file = pFile.smart_dynamic_cast<PPTX::WrapperFile>();

				if (file.IsInit())
				{
					if (file->GetWrittenStatus() == false)
					{
						OOX::CPath defdir	= pFile->DefaultDirectory();
						OOX::CPath name		= pFile->DefaultFileName();

						//name = name + max_name_index(curdir, name.string());

						OOX::CSystemUtility::CreateDirectories(directory / defdir);
						pFile->write(directory / defdir / name, directory, content);
						rels.Registration(pPair->first, pFile->type(), defdir / name);
					}
					else
					{
						OOX::CPath defdir	= pFile->DefaultDirectory();
						OOX::CPath name		= file->GetWrittenFileName();

						rels.Registration(pPair->first, pFile->type(), defdir / name);
					}
				}
				else
				{
					OOX::CPath defdir	= pFile->DefaultDirectory();
					OOX::CPath name		= pFile->DefaultFileName();

					OOX::CSystemUtility::CreateDirectories(directory / defdir);
					pFile->write(directory / defdir / name, directory, content);
					rels.Registration(pPair->first, pFile->type(), defdir / name);
				}
			}
			else
			{
				rels.Registration(pPair->first, pExt);
			}
		}
	}

	void FileContainer::WrittenSetFalse()
	{
		for (std::map<std::wstring, smart_ptr<OOX::File>>::const_iterator pPair = m_mContainer.begin(); pPair != m_mContainer.end(); ++pPair)
		{
			smart_ptr<OOX::File> pFile = pPair->second;

			smart_ptr<PPTX::WrapperFile>	pWrapFile = pFile.smart_dynamic_cast<PPTX::WrapperFile>();
			smart_ptr<PPTX::FileContainer>	pWrapCont = pFile.smart_dynamic_cast<PPTX::FileContainer>();

			if (pWrapFile.is_init() && !pWrapFile->GetWrittenStatus())
			{
				pWrapFile->WrittenSetFalse();	

				if (pWrapCont.is_init())
				{
					pWrapCont->WrittenSetFalse();
				}
			}
		}
	}

	void CCommonRels::_read(const OOX::CRels& rels, const OOX::CPath& path)
	{
		size_t nCount = rels.m_arrRelations.size();
		for (size_t i = 0; i < nCount; ++i)
		{
			OOX::Rels::CRelationShip* pRelation = rels.m_arrRelations[i];

			OOX::CPath normPath = CorrectPathRels(path, pRelation);

			smart_ptr<OOX::File> _file = PPTX::FileFactory::CreateFilePPTX_OnlyMedia(normPath, *pRelation);
			Add(pRelation->rId(), _file);	
		}
	}

	void CCommonRels::_read(const OOX::CPath& filename)
	{
		OOX::CRels rels(filename);
		OOX::CPath path = filename.GetDirectory();
		_read(rels, path);
	}
} // namespace PPTX
