#include "File.h"

namespace FileSystem {
    bool File::Exists(LPCTSTR path) {
		WIN32_FIND_DATA findData;
		ZeroMemory(&findData, sizeof(findData));

		HANDLE handle = ::FindFirstFile(path, &findData);

		bool fileExists = true;
		if (handle == INVALID_HANDLE_VALUE)
			fileExists = false;
		FindClose(handle);

		return fileExists;
	}
    bool File::Exists(const String& path) {
        return Exists(path.c_str());
    }

    // wtf?
	void File::Create(LPCTSTR path) {
		CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
    void File::Create(const String& path) {
        Create(path.c_str());
	}
}