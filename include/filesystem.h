/*	======================================================================

	Title:		Windows File System Helper Library
	Author:		mariokart64n
	Version:	0.1
	Date:		February 01 2019

	""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

	Description:
		General helper class for accessing files on windows

	""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

	Change Log:
        [2022-11-27]
            rearranged, duplicates removed, some tweaks

        [2019-01-28]
            wrote it!

	======================================================================	*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iostream>
#include <cmath>
#include <stack> // Include the stack header
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <shobjidl.h> // For IFileDialog
#include <combaseapi.h>

#include <tchar.h>
#include <strings.h>
#include <wchar.h>

using namespace std;

//#include "stringext.h"
//#include "wstringext.h"

/**
 * Adds a trailling forward slash to the filepath
 * @param <filepath> <add or remove the ending slash>
 * @return Example: "C:\\tmp\\"
 */
std::string fixTrailingSlash (std::string path, bool addSlash = true);

/**
 * Adds a trailling forward slash to the filepath
 * @param <filepath> <add or remove the ending slash>
 * @return Example: "C:\\tmp\\"
 */
std::wstring fixTrailingSlashW (std::wstring path, bool addSlash = true);


// File Name Parsing
namespace getFilename {

    /**
     * Splits Path from FullFilePath
     * @param filename_string
     * @return (name with extension) returns: "myImage.jpg"
     */
    std::string FromPath(const std::string &str);

    /**
     * Splits Path from FullFilePath
     * @param filename_string
     * @return (Path) returns: "g:\subdir1\subdir2\"
     */
    std::string Path (const std::string &str);

    /**
     * Splits File Name from FullFilePath
     * @param filename_string
     * @return (Filename) returns: "myImage"
     */
    std::string File(const std::string &str);

    /**
     * Splits File Extension from FullFilePath
     * @param filename_string
     * @return (Extension) returns: ".jpg"
     */
    std::string Type(std::string const & path, bool useWinAPI = true);

    /**
     * Splits Path from FullFilePath
     * @param filename_string
     * @return (name with extension) returns: "myImage.jpg"
     */
    std::wstring FromPathW(const std::wstring &str);

    /**
     * Splits Path from FullFilePath
     * @param filename_string
     * @return (Path) returns: "g:\subdir1\subdir2\"
     */
    std::wstring PathW (const std::wstring &str);

    /**
     * Splits File Name from FullFilePath
     * @param filename_string
     * @return (Filename) returns: "myImage"
     */
    std::wstring FileW(const std::wstring &str);

    /**
     * Splits File Extension from FullFilePath
     * @param filename_string
     * @return (Extension) returns: ".jpg"
     */
    std::wstring TypeW (std::wstring const &path, bool useWinAPI = true);
    }

namespace sysinfo {

    /**
     * Running Path of the Application
     * @return returns: "g:\subdir1\subdir2\"
     */
    std::string currentdir();

    /**
     * Running Path of the Application
     * @return returns: "g:\subdir1\subdir2\"
     */
    std::wstring currentdirW();
    }

namespace os {

    /**
     * Checks if Path is Absolute and is Not Relative
     * @param filepath
     * @return fullpath: "g:\subdir1\subdir2\image.jpg"
     */
    bool isPathAbsolute (std::string path);

    /**
     * Checks if Path is Absolute and is Not Relative
     * @param filepath
     * @return fullpath: "g:\subdir1\subdir2\image.jpg"
     */
    bool isPathAbsoluteW (std::wstring path);

    /**
     * Combines a File Path with Relative File Path
     * @param <relative path> <file path> :: {"img\pic.jpg", "C:\"}
     * @return <fullpath> "C:\img\pic.jpg"
     */
    std::string resolveToAbsolute  (std::string relPath, std::string basePath);

    /**
     * Combines a File Path with Relative File Path
     * @param <relative path> <file path> :: {"img\pic.jpg", "C:\"}
     * @return <fullpath> "C:\img\pic.jpg"
     */
    std::wstring resolveToAbsoluteW (std::wstring relPath, std::wstring basePath);


    /**
     * Checks if File or Folder Exists
     * @param <file path>
     * @return Returns true if the file exists, false otherwise.
     */
	bool doesFileExist (std::string fileName);

    /**
     * Checks if File or Folder Exists
     * @param <file path>
     * @return Returns true if the file exists, false otherwise.
     */
	bool doesFileExistW (std::wstring fileName);

    /**
     * Checks if Full File Path is a Directory
     * @param <file path>
     * @return Returns true if path is a directory
     */
	bool isDirectory (std::string dirName_in);

    /**
     * Checks if Full File Path is a Directory
     * @param <file path>
     * @return Returns true if path is a directory
     */
	bool isDirectoryW (std::wstring dirName_in);

    /**
     * Opens a file open dialog
     * @param <file filter> <parent window> <flags>
     * @return Returns file path as string if selection is made
     */
    std::string getOpenFileName(const char* filter = "All Files (*.*)\0*.*\0", HWND owner = NULL, unsigned long flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);

    /**
     * Opens a file open dialog
     * @param <file filter> <parent window> <flags>
     * @return Returns file path as string if selection is made
     */
    std::wstring getOpenFileNameW(const wchar_t* filter = L"All Files (*.*)\0*.*\0", HWND owner = NULL, unsigned long flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);

    /**
     * Opens a file save dialog
     * @param <file filter> <parent window>
     * @return Returns file path as string if file is saved
     */
    std::string getSaveFileName(const char* filter = "All Files (*.*)\0*.*\0", HWND owner = NULL, const char* defaultFileName = "");

    /**
     * Opens a file save dialog
     * @param <file filter> <parent window>
     * @return Returns file path as string if file is saved
     */
    std::wstring getSaveFileNameW(const wchar_t* filter = L"All Files (*.*)\0*.*\0", HWND owner = NULL);

    /**
     * Opens a folder open dialog
     * @param <default folder> <dialog title> <flags>
     * @return Returns folder path as string if folder was selected
     */
    std::string getSavePath(std::string defaultPath = "", const char* szCaption = "Browse for folder...", unsigned int flags = BIF_USENEWUI, HWND hOwner = NULL);

    /**
     * Opens a folder open dialog
     * @param <default folder> <dialog title> <flags>
     * @return Returns folder path as string if folder was selected
     */
    std::wstring getSavePathW(std::wstring defaultPath = L"", const wchar_t* szCaption = L"Browse for folder...", unsigned int flags = BIF_USENEWUI, HWND hOwner = NULL);

    /**
     * Get Files in a directory
     * @param <search pattern> Example: getFiles("C:\\tmp\\*.jpg");
     * @return Returns an array of file names that match the given wild-card path name.
     */
    std::vector<std::string> getFiles (std::string pattern);

    /**
     * Get Files in a directory
     * @param <search pattern> Example: getFiles("C:\\tmp\\*.jpg");
     * @return Returns an array of file names that match the given wild-card path name.
     */
    std::vector<std::wstring> getFilesW (std::wstring pattern);

    /**
     * Get Files in a directory and it's sub folders
     * @param <search pattern> Example: getFilesRecursive("C:\\tmp\\*.jpg");
     * @return Returns an array of file names that match the given wild-card path name.
     */
    std::vector<std::string> getFilesRecursive (const std::string& pattern);

    /**
     * Get Files in a directory and it's sub folders
     * @param <search pattern> Example: getFilesRecursive("C:\\tmp\\*.jpg");
     * @return Returns an array of file names that match the given wild-card path name.
     */
    std::vector<std::wstring> getFilesRecursiveW (std::wstring pattern);

    // getDirectories

    // getDirectoriesW

    /**
     * Creates a new directory with the given name.
     * @param <directory path> Example: makeDir("C:\\tmp\\");
     * @return Returns true on success, false on failure
     */
    bool makeDir(std::string sPath);

    /**
     * Creates a new directory with the given name.
     * @param <directory path> Example: makeDir("C:\\tmp\\");
     * @return Returns true on success, false on failure
     */
    bool makeDirW(std::wstring wsPath);

    /**
     * Deletes a file
     * @param <search pattern> Example: getFilesRecursive("C:\\tmp\\*.jpg");
     * @return Returns true on success, false on failure
     */
    bool deleteFile (std::string file);

    /**
     * Deletes a file
     * @param <search pattern> Example: getFilesRecursive("C:\\tmp\\*.jpg");
     * @return Returns true on success, false on failure
     */
    bool deleteFileW (std::wstring file);

    /**
     * Renames a file
     * @param <Old Filepath> <new Filepath> Example: renameFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
    bool renameFile (std::string old_name, std::string new_name);

    /**
     * Renames a file
     * @param <Old Filepath> <new Filepath> Example: renameFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
    bool renameFileW (std::wstring old_name, std::wstring new_name);

   /**
     * Copies a file
     * @param <Source File> <New File> Example: copyFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
	bool copyFile (std::string existing_filename, std::string target_filename);

   /**
     * Copies a file
     * @param <Source File> <New File> Example: copyFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
	bool copyFileW (std::wstring existing_filename, std::wstring target_filename);

   /**
     * Moves a file
     * @param <Source File> <Target File> Example: copyFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
	bool moveFile (std::string existing_filename, std::string target_filename);

   /**
     * Moves a file
     * @param <Source File> <Target File> Example: copyFile("C:\\tmp\\old.jpg", "C:\\tmp\\new.jpg");
     * @return Returns true on success, false on failure
     */
	bool moveFileW (std::wstring existing_filename, std::wstring target_filename);

   /**
     * Returns File Size of given file
     * @param <Filepath>
     * @return Returns size
     */
	size_t getFileSize (std::string fileName);

   /**
     * Returns File Size of given file
     * @param <Filepath>
     * @return Returns size
     */
	size_t getFileSizeW(std::wstring fileName);

   /**
     * Pauses Console
     * @param <useSystem> uses windows API
     * @return None
     */
    void pause (bool useSystem = false);

   /**
     * Sends a Message Prompt
     * @return None
     */
    int messageBox(std::string lpText = "TEXT", std::string lpCaption = "Caption", UINT uType = MB_OKCANCEL, UINT uIconResID = 0, HWND hWnd = 0);
    INT_PTR CALLBACK CustomMsgBoxProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    int CustomMessageBox(HWND hWnd, const std::string& text, const std::string& caption, UINT uType);
    /**
     * Sends a Yes or No Dialog Prompt
     * @param <message> <title> <play beep> uses windows API
     * @return <int> {IDYES, IDNO}
     */
    int queryBox (std::string lpText = "TEXT", std::string lpCaption = "Caption", bool beep = false);

    /**
     * \brief Sends a Yes, No, Cancel Dialog Prompt
     * \param <message> <title> <play beep> uses windows API
     * \return <int> {IDYES, IDNO, IDCANCEL}
     */
    int yesNoCancelBox (std::string lpText = "TEXT", std::string lpCaption = "Caption", bool beep = false);


bool isAbsolutePath(const std::wstring& path);

std::wstring getAbsolutePath(const std::wstring& relativePath);


    }




#endif // FILESYSTEM_H
