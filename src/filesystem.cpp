#include "filesystem.h"

// Win32 incompatibilities
#if defined(WIN32) && !defined(__GNUC__)
    #define wcsncasecmp _wcsnicmp
    #define wcscasecmp _wcsicmp
    static inline bool isnan(double x) { return x != x; }
    static inline bool isinf(double x) { return !isnan(x) && isnan(x - x); }
#endif


std::string fixTrailingSlash (std::string path, bool addSlash) {
    if (!path.empty()) {
        if ((*path.rbegin() == '/' || *path.rbegin() == '\\')) {
            if (!addSlash) {
                path = path.substr(0, path.length() - 1);
                }
            }
        else if (addSlash) {
            path += '\\';
            }
        }
    return path;
    }

std::wstring fixTrailingSlashW (std::wstring path, bool addSlash) {
    if (!path.empty()) {
        if ((*path.rbegin() == L'/' || *path.rbegin() == L'\\')) {
            if (!addSlash) {
                path = path.substr(0, path.length() - 1);
                }
            }
        else if (addSlash) {
            path += L'\\';
            }
        }
    return path;
    }

std::string getFilename::FromPath(const std::string &str) {
    // filenameFromPath file  -- returns: "myImage.jpg"
    size_t found;
    std::string strt;
    found = str.find_last_of("/\\");
    if (found < str.size()) {
        strt = str.substr(found + 1, -1);
        return strt;
        }
    else {
        return str;
        }
    }

std::string getFilename::Path (const std::string &str) {
	// getFilenamePath file   -- returns: "g:\subdir1\subdir2\"
	unsigned int found = 0;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found != std::string::npos) {
		strt = str.substr(0, found);
		return (strt + "\\");
		}
	else {
		return "";
		}
	}

std::string getFilename::File(const std::string &str) {
	// getFilenameFile file   -- returns: "myImage"
	unsigned int found = 0;
	std::string strt = str;
	found = str.find_last_of("/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find_last_of(".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
			}
		}
	return strt;
	}

std::string getFilename::Type(std::string const & path, bool useWinAPI) {
    std::string ext = "";
    if (!useWinAPI) {

        // Find the last dot, if any.
        size_t dotIdx = path.find_last_of( "." );
        if ( dotIdx != std::string::npos ) {
            // Find the last directory separator, if any.
            size_t dirSepIdx = path.find_last_of( "/\\" );

            // If the dot is at the beginning of the file name, do not treat it as a file extension.
            // e.g., a hidden file:  ".alpha".
            // This test also incidentally avoids a dot that is really a current directory indicator.
            // e.g.:  "alpha/./bravo"
            if ( dotIdx > dirSepIdx + 1 ) {
                ext = path.substr( dotIdx );
                }
            }
        return ext;
        } else {return std::string(PathFindExtensionA(path.c_str()));}
    return ext;
    }

std::wstring getFilename::FromPathW(const std::wstring &str) {
    // filenameFromPath file  -- returns: "myImage.jpg"
    size_t found;
    std::wstring strt;
    found = str.find_last_of(L"/\\");
    if (found < str.size()) {
        strt = str.substr(found + 1, -1);
        return strt;
        }
    else {
        return str;
        }
    }

std::wstring getFilename::PathW (const std::wstring &str) {
	// getFilenamePath file   -- returns: "g:\subdir1\subdir2\"
	size_t found;
	std::wstring strt;
	found = str.find_last_of(L"/\\");
	if (found != std::wstring::npos) {
		strt = str.substr(0, found);
		return (strt + L"\\");
		}
	else {
		return L"";
		}
	}

std::wstring getFilename::FileW(const std::wstring &str) {
	// getFilenameFile file   -- returns: "myImage"
	size_t found;
	std::wstring strt = str;
	found = str.find_last_of(L"/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find_last_of(L".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
			}
		}
	return strt;
	}

std::wstring getFilename::TypeW(std::wstring const & path, bool useWinAPI) {
    std::wstring ext = L"";
    if (!useWinAPI) {

        // Find the last dot, if any.
        size_t dotIdx = path.find_last_of( L"." );
        if ( dotIdx != std::wstring::npos ) {
            // Find the last directory separator, if any.
            size_t dirSepIdx = path.find_last_of( L"/\\" );

            // If the dot is at the beginning of the file name, do not treat it as a file extension.
            // e.g., a hidden file:  ".alpha".
            // This test also incidentally avoids a dot that is really a current directory indicator.
            // e.g.:  "alpha/./bravo"
            if ( dotIdx > dirSepIdx + 1 ) {
                ext = path.substr( dotIdx );
                }
            }
        return ext;
        } else {return std::wstring(PathFindExtensionW(path.c_str()));}
    return ext;
    }

std::string sysinfo::currentdir() {
    // https://stackoverflow.com/a/19535628
    char result[ MAX_PATH ];
    return getFilename::Path(std::string(result, GetModuleFileNameA( NULL, result, MAX_PATH )));
    }

std::wstring sysinfo::currentdirW() {
    // https://stackoverflow.com/a/19535628
    wchar_t result[ MAX_PATH ];
    return getFilename::PathW(std::wstring(result, GetModuleFileNameW( NULL, result, MAX_PATH )));
    }

bool os::isPathAbsolute (std::string path) {
    char full[_MAX_PATH];
    char path_slash[_MAX_PATH];
    char *p;
    strncpy(path_slash, path.c_str(), _MAX_PATH);
    path_slash[_MAX_PATH-1] = '\0';
    for (p = path_slash; p < path_slash + strlen(path_slash); p++) {
        if (*p == '/') {
            *p = '\\';
            }
        }
    return (strcasecmp(_fullpath(full, path_slash, _MAX_PATH), path_slash) == 0);
    }

bool os::isPathAbsoluteW (std::wstring path) {
    wchar_t full[_MAX_PATH];
    wchar_t path_slash[_MAX_PATH];
    wchar_t *p;
    wcsncpy(path_slash, path.c_str(), _MAX_PATH);
    path_slash[_MAX_PATH-1] = '\0';
    for (p = path_slash; p < path_slash + wcslen(path_slash); p++) {
        if (*p == '/') {
            *p = '\\';
            }
        }
    return (_wcsicmp(_wfullpath(full, path_slash, _MAX_PATH), path_slash) == 0);
    }

std::string os::resolveToAbsolute  (std::string relPath, std::string basePath) {

    // Buffer to hold combined path.
    char buffer_1[MAX_PATH] = "";
    char *lpStr1;
    lpStr1 = buffer_1;

    PathCombineA(lpStr1,basePath.c_str(),relPath.c_str());

    return lpStr1;
    }

std::wstring os::resolveToAbsoluteW (std::wstring relPath, std::wstring basePath) {

    // Buffer to hold combined path.
    wchar_t buffer_1[MAX_PATH] = L"";
    wchar_t *lpStr1;
    lpStr1 = buffer_1;

    PathCombineW(lpStr1,basePath.c_str(),relPath.c_str());

    return lpStr1;
    }


bool os::doesFileExist (std::string fileName) {
    //std::ifstream infile(fileName.c_str());
    //return infile.good();
    //DWORD dwAttrib = GetFileAttributesA(fileName.c_str());
    //return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    //return (dwAttrib != INVALID_FILE_ATTRIBUTES);
    return PathFileExistsA(fileName.c_str());
    }

bool os::doesFileExistW (std::wstring fileName) {
    /*	Old function that used fstream

    std::ifstream infile(fileName.c_str());
    return infile.good();
    */

    /*	Old function that used File Attributes

    DWORD dwAttrib = GetFileAttributesW((wchar_t*)fileName.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    */
    // add to link -lshlwapi
    return PathFileExistsW((wchar_t*)fileName.c_str());
    }

bool os::isDirectory (std::string dirName_in) {
    //DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    /*
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        return false;  //something is wrong with your path!
        }
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
        return true;   // this is a directory!
        }
    return false;    // this is not a directory!
    */
    return PathIsDirectoryA(dirName_in.c_str());
    }

bool os::isDirectoryW (std::wstring dirName_in) {
    //DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    /*
    DWORD ftyp = GetFileAttributesW((wchar_t*)dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        return false;  //something is wrong with your path!
        }
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
        return true;   // this is a directory!
        }
    return false;    // this is not a directory!
    */
    return PathIsDirectoryW(dirName_in.c_str());
    }

std::string os::getOpenFileName(const char* filter, HWND owner, unsigned long flags) {
    // add to link -lcomdlg32 -lole32
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = flags;
    ofn.lpstrDefExt = "";
    std::string fileNameStr = "";
    if (GetOpenFileNameA(&ofn)) {
        fileNameStr = fileName;
        }
    return fileNameStr;
    }

std::wstring os::getOpenFileNameW(const wchar_t* filter, HWND owner, unsigned long flags) {
    // add to link -lcomdlg32 -lole32
    OPENFILENAMEW ofn = {0}; // Zero initialize the structure
    std::cout << "MAX_PATH: " << MAX_PATH << std::endl;
    wchar_t fileName[MAX_PATH] = L"";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Select a File";
    ofn.Flags = flags | OFN_ENABLESIZING | OFN_EXPLORER; // Add sensible default flags
    // ofn.lpstrDefExt = L"ext"; // Set a default extension if needed

    if (GetOpenFileNameW(&ofn)) {
        return fileName;
        }
    else {
        DWORD error = CommDlgExtendedError();
        if (error != 0) {
            std::wcerr << L"Common dialog error: " << error << std::endl;
            }
        return L"";
        }
    }

std::string os::getSaveFileName(const char* filter, HWND owner, const char* defaultFileName ) {
    // Add to link -lcomdlg32 -lole32
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";
    if (defaultFileName && strlen(defaultFileName) < MAX_PATH) {
        strcpy(fileName, defaultFileName);
    }

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";

    std::string fileNameStr;
    if (GetSaveFileNameA(&ofn)) {
        fileNameStr = fileName;
    }

    return fileNameStr;
}


std::wstring os::getSaveFileNameW(const wchar_t* filter, HWND owner) {
    // add to link -lcomdlg32 -lole32
    OPENFILENAMEW ofn;
    wchar_t fileName[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"";
    std::wstring fileNameStr;
    if (GetSaveFileNameW(&ofn)) {
        fileNameStr = fileName;
        }
    return fileNameStr;
    }

// Local Function for os::getSavePath
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if(uMsg == BFFM_INITIALIZED) {
        std::string tmp = (const char *) lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessageA(hwnd, BFFM_SETSELECTION, TRUE, lpData);
        }
    return 0;
    }

/*
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    char dir[MAX_PATH];
    ITEMIDLIST *lpid;
        HWND hEdit;

        switch (uMsg){
        case BFFM_INITIALIZED:
                SendMessageA(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
                break;
        case BFFM_VALIDATEFAILED:
                MessageBoxA(hWnd,(char*)lParam,"error",MB_OK);
                hEdit=FindWindowExA(hWnd,NULL,"EDIT",NULL);
                SetWindowTextA(hEdit,"");
                return 1;
                break;

        case BFFM_SELCHANGED:
                lpid=(ITEMIDLIST *)lParam;
                SHGetPathFromIDListA(lpid,dir);
                hEdit=FindWindowExA(hWnd,NULL,"EDIT",NULL);
                SetWindowTextA(hEdit,dir);
                break;
        }
        return 0;
    }
*/

// Local Function for os::getSavePathW
int CALLBACK BrowseCallbackProcW(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if(uMsg == BFFM_INITIALIZED) {
        std::wstring tmp = (const wchar_t *) lpData;
        std::wcout << L"path: " << tmp << std::endl;
        SendMessageW(hwnd, BFFM_SETSELECTION, TRUE, lpData);
        }
    return 0;
    }

std::string os::getSavePath(std::string defaultPath, const char* szCaption, unsigned int flags, HWND hOwner) {
    std::string folderpath = "";
    BROWSEINFOA bi;
    memset(&bi, 0, sizeof(bi));
    bi.ulFlags   = flags | BIF_NEWDIALOGSTYLE;
    bi.hwndOwner = hOwner;
    bi.lpszTitle = szCaption;
    bi.lpfn      = BrowseCallbackProc;
    bi.lParam    = (LPARAM)defaultPath.c_str();

    if (flags & BIF_USENEWUI) {
        OleInitialize(NULL);
    }

    LPITEMIDLIST pIDL = ::SHBrowseForFolderA(&bi);

    if(pIDL != NULL) {
        char buffer[MAX_PATH] = {'\0'};
        if(SHGetPathFromIDListA(pIDL, buffer) != 0) {
            folderpath = buffer;
        }
        CoTaskMemFree(pIDL);
    }

    if (flags & BIF_USENEWUI) {
        OleUninitialize();
    }

    return folderpath;
}

std::wstring os::getSavePathW(std::wstring defaultPath, const wchar_t* szCaption, unsigned int flags, HWND hOwner) {
    std::wstring folderpath = L"";
    BROWSEINFOW bi;
    memset(&bi, 0, sizeof(bi));
    bi.ulFlags   = flags | BIF_NEWDIALOGSTYLE;
    bi.hwndOwner = hOwner;
    bi.lpszTitle = szCaption;
    bi.lpfn      = BrowseCallbackProcW;
    bi.lParam    = (LPARAM)defaultPath.c_str();

    if (flags & BIF_USENEWUI) {
        OleInitialize(NULL);
    }

    LPITEMIDLIST pIDL = ::SHBrowseForFolderW(&bi);

    if(pIDL != NULL) {
        wchar_t buffer[MAX_PATH] = {'\0'};
        if(SHGetPathFromIDListW(pIDL, buffer) != 0) {
            folderpath = buffer;
        }
        CoTaskMemFree(pIDL);
    }

    if (flags & BIF_USENEWUI) {
        OleUninitialize();
    }

    return folderpath;
}

std::vector<std::string> os::getFiles (std::string pattern) {
    std::vector<std::string> v;
    std::string path = getFilename::Path(pattern);
    WIN32_FIND_DATAA data;
    HANDLE hFind;
    if ((hFind = FindFirstFileA(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
    do {v.insert( v.end(), (path + data.cFileName));}
    while (FindNextFileA(hFind, &data) != 0);
        FindClose(hFind);
        }
    return v;
    }

std::vector<std::wstring> os::getFilesW (std::wstring pattern) {
    std::vector<std::wstring> v;
    std::wstring path = getFilename::PathW(pattern);
    WIN32_FIND_DATAW data;
    HANDLE hFind;
    if ((hFind = FindFirstFileW(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
    do {v.insert( v.end(), (path + data.cFileName));}
    while (FindNextFileW(hFind, &data) != 0);
        FindClose(hFind);
        }
    return v;
    }

std::vector<std::string> os::getFilesRecursive(const std::string& pattern) {
    std::vector<std::string> files;
    std::string folder = getFilename::Path(pattern);
    WIN32_FIND_DATAA fd;
    std::stack<std::string> folders;
    folders.push(folder);

    while (!folders.empty()) {
        std::string current_folder = folders.top();
        folders.pop();

        std::string search_pattern = current_folder + "\\*.*";
        HANDLE hFind = ::FindFirstFileA(search_pattern.c_str(), &fd);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                        folders.push(current_folder + "\\" + fd.cFileName);
                    }
                } else {
                    files.push_back(current_folder + "\\" + fd.cFileName);
                }
            } while (::FindNextFileA(hFind, &fd));

            ::FindClose(hFind);
        }
    }

    return files;
}


std::vector<std::wstring> getFilesRecursiveW (std::wstring pattern) {
    vector<wstring> files;
    std::wstring folder = getFilename::PathW(pattern);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = ::FindFirstFileW(pattern.c_str(), &fd);
    std::wstring tmp;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!(!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L".."))) {
                    tmp = folder;
                    tmp = tmp + fd.cFileName;
                    std::vector<std::wstring> arr = getFilesRecursiveW(tmp);
                    files.reserve(files.size() + arr.size());                // preallocate memory without erase original data
                    files.insert(files.end(), arr.begin(), arr.end());         // add arr;
                    }
                } else {files.push_back(folder + fd.cFileName);}
            }
        while (::FindNextFileW(hFind, &fd));
        ::FindClose(hFind);
        }
    return files;
    }

bool os::makeDir(std::string sPath) {
    // https://www.codeguru.com/cpp/w-p/files/article.php/c4439/CreatePath--Full-Path-Creation-wstring-version.htm
    unsigned long attr;
    size_t pos;
    bool result = false;
    std::string tmp;

    // Check for trailing slash:
    /* I dont understand what this code is doing
    pos = sPath.find_last_of(L"\\");

    if (sPath.length() == pos + 1) { // last character is "\"
        sPath.resize(pos);
        }
    */
    if (sPath.length() > 0) {
        if (sPath[sPath.length() - 1] != '\\') {
            sPath += "\\";
            }
        // Look for existing object:
        attr = GetFileAttributesA(sPath.c_str());

        //std::wcout << "CreatingFolder:\t" << sPath << std::endl;
        //std::wcout << "Attribute:\t" << attr << std::endl;
        if (attr == 0xFFFFFFFF) { // doesn't exist yet - create it!
            pos = sPath.find_last_of("\\", sPath.length() - 2);
            //pos = sPath.find_first_of(L"\\/");
            //std::wcout << "POS:\t" << pos << std::endl;
            if (pos != std::string::npos) {
                // Create parent dirs:
                //std::wcout << L"Build\t" << sPath.substr(0, pos + 1) << std::endl;
                result = makeDir(sPath.substr(0, pos + 1));
                }
            // Create node:
            result = result && CreateDirectoryA(sPath.c_str(), NULL);
            CreateDirectoryA(sPath.c_str(), NULL);
            }
        else if (FILE_ATTRIBUTE_DIRECTORY != attr) {  // object already exists, but is not a dir
            //SetLastError(ERROR_FILE_EXISTS);
            result = false;
            }
        }
    return result;
    }

bool os::makeDirW(std::wstring wsPath) {
    // https://www.codeguru.com/cpp/w-p/files/article.php/c4439/CreatePath--Full-Path-Creation-wstring-version.htm
    if (wsPath.empty()) {
        return false;
        }

    // Normalize path by ensuring it ends with a backslash
    std::wstring normalizedPath = wsPath;
    if (normalizedPath.back() != L'\\') {
        normalizedPath += L"\\";
        }

    // Check if the directory already exists
    DWORD attr = GetFileAttributesW(normalizedPath.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        // Directory already exists
        return true;
        }

    // Find the position of the last backslash, ignoring the trailing backslash
    size_t lastSlashPos = normalizedPath.find_last_of(L"\\", normalizedPath.length() - 2);
    if (lastSlashPos == std::wstring::npos) {
        return false; // No backslash found, invalid path
        }

    // Recursively create parent directory
    if (!makeDirW(normalizedPath.substr(0, lastSlashPos))) {
        return false;
        }

    // Create the directory
    return CreateDirectoryW(normalizedPath.c_str(), NULL) != 0;
    }

bool os::deleteFile (std::string file) {
    return DeleteFileA(file.c_str());
    }

bool os::deleteFileW (std::wstring file) {
    return DeleteFileW(file.c_str());
    }

bool os::renameFile (std::string old_name, std::string new_name) {
    return rename(old_name.c_str(), new_name.c_str());
    }

bool os::renameFileW (std::wstring old_name, std::wstring new_name) {
    return _wrename(old_name.c_str(), new_name.c_str());
    }

bool os::copyFile (std::string existing_filename, std::string target_filename) {
    if (CopyFileA (existing_filename.c_str(), target_filename.c_str(), true)) {
        return true;
        }
    return false;
    }

bool os::copyFileW (std::wstring existing_filename, std::wstring target_filename) {
    if (CopyFileW ((wchar_t*)existing_filename.c_str(), (wchar_t*)target_filename.c_str(), true)) {
        return true;
        }
    return false;
    }

bool os::moveFile (std::string existing_filename, std::string target_filename) {
    return MoveFileA (existing_filename.c_str(), target_filename.c_str());
    }

bool os::moveFileW (std::wstring existing_filename, std::wstring target_filename) {
    return MoveFileW ((wchar_t*)existing_filename.c_str(), (wchar_t*)target_filename.c_str());
    }

size_t os::getFileSize (std::string fileName) {
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA((char*)fileName.c_str(), GetFileExInfoStandard, &fad))
        return -1; // error condition, could call GetLastError to find out more
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return (size_t)size.QuadPart;
    }

size_t os::getFileSizeW(std::wstring fileName) {
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExW((wchar_t*)fileName.c_str(), GetFileExInfoStandard, &fad))
        return -1; // error condition, could call GetLastError to find out more
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return (size_t)size.QuadPart;
    }

void os::pause (bool useSystem) {
    if (useSystem) {
        system("pause");
        }
    else {
        std::cout << "Press ENTER to continue...";
        std::cin.clear();
        std::cin.sync();
        std::cin.get();
        //getchar();
        }
    }

int os::messageBox(std::string lpText, std::string lpCaption, UINT uType, UINT uIconResID, HWND hWnd) {
    // https://forums.codeguru.com/showthread.php?407479-How-to-use-a-custom-icon-in-a-message-box&p=1497200#post1497200
    // need to add linker option -lwinmm
    // MessageBox( 0, "Failed to open in notepad",  "Message", MB_OK + MB_ICONHAND );
    //PlaySound(sound_wow, GetModuleHandle(NULL), SND_MEMORY | SND_ASYNC);
    MSGBOXPARAMSA mbp;
    mbp.cbSize = sizeof(MSGBOXPARAMS);
    mbp.hwndOwner = hWnd;
    mbp.hInstance = GetModuleHandle(NULL);
    mbp.lpszText = lpText.c_str();
    mbp.lpszCaption = lpCaption.c_str();
    if (uIconResID > 0) {
        mbp.dwStyle = uType | MB_USERICON;
        mbp.lpszIcon = MAKEINTRESOURCEA(uIconResID);
        }
    else {
        mbp.dwStyle = uType;
        }
    mbp.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
    mbp.lpfnMsgBoxCallback = NULL;
    mbp.dwContextHelpId = 0;
    return MessageBoxIndirectA(&mbp);
    }


int os::queryBox (std::string lpText, std::string lpCaption, bool beep) {
    return MessageBoxA(NULL, lpText.c_str(), lpCaption.c_str(), MB_YESNO);
    }

int os::yesNoCancelBox (std::string lpText, std::string lpCaption, bool beep) {
    return MessageBoxA(NULL, lpText.c_str(), lpCaption.c_str(), MB_YESNO);
    }

bool os::isAbsolutePath(const std::wstring& path) {
    return path.size() > 2 &&
           ((path[1] == L':' && (path[2] == L'\\' || path[2] == L'/')) || // Windows drive letter, e.g., C:/
            (path[0] == L'\\' && path[1] == L'\\')); // Network path, e.g., \\server\share
}

std::wstring os::getAbsolutePath(const std::wstring& relativePath) {
    WCHAR buffer[MAX_PATH];
    DWORD length = GetFullPathNameW(relativePath.c_str(), MAX_PATH, buffer, nullptr);
    if (length == 0 || length > MAX_PATH) {
        // Error handling: GetFullPathNameW failed or path is too long
        std::wcerr << L"Failed to get full path from " << relativePath << std::endl;
        return L"";
    }
    return std::wstring(buffer);
}
