//#include <iostream>
//#include <string>
#include <cstring>
//#include <cstddef>
//#include <windows.h>
//#include <shlwapi.h>
//#include <cmath>
//#include <tchar.h>
//#include <vector>
//#include <iomanip> // needed for setprecision
//#include <sstream>
//#include <fstream>
//#include <time.h>	// needed for get_part_date

#include <windows.h>
#include <math.h>       /* pow */
#include "bytestream.h"


//unsigned int bit::and (unsigned int a, unsigned int b) {
//	return (a & b);
//	}

//unsigned int bit::or (unsigned int a, unsigned int b) {
//	return (a | b);
//	}

//unsigned int bit::xor (unsigned int a, unsigned int b) {
//	return (a ^ b);
//	}

//unsigned int bit::not (unsigned int a) {
//	return ~a;
//	}
uint16_t fcs16_lut[256] = {
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
	0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
	0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
	0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
	0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
	0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
	0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
	0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
	0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
	0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
	0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
	0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
	0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
	0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
	0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
	0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
	0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
	0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
	0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
	0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
	0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
	0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
	0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
	0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
	0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
	0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
	0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
	0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
	0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
	0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
	0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
	0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
	};
unsigned int bits::shift (unsigned int number, int offset) {
	if (offset < 0) {
		return (number >> offset);
		}
	else {
		return (number << offset);
		}
	}

bool bits::get (int number, int position) {
	return (number & (1 << (position - 1))) ? true : false;
	}

unsigned int bits::flip (unsigned int number, unsigned int position) {
	return (number ^ 1UL << (position - 1));
	}

unsigned int bits::set (unsigned int n, unsigned int p, bool b) {
	p -= 1;
	int mask = 1 << p;
	return ((n & ~mask) | ((int)b << p));
	}
unsigned long bits::swapBytes (unsigned long num, const unsigned int pos1, const unsigned int pos2) {
	unsigned long v1 = (num & (0xFF << ((pos1 - 1) * 8))) >> ((pos1 - 1) * 8);
	unsigned long v2 = (num & (0xFF << ((pos2 - 1) * 8))) >> ((pos2 - 1) * 8);
	num -= num & (0xFF << ((pos2 - 1) * 8));
	num -= num & (0xFF << ((pos1 - 1) * 8));
	num += v1 << ((pos2 - 1) * 8);
	num += v2 << ((pos1 - 1) * 8);
	return num;
	}
float bits::IntAsFloat (unsigned long b) {
	float result = 0.0;
	memcpy(&result, &b, 4);
	return result;
	}
unsigned long bits::FloatAsInt (float b) {
	unsigned long result = 0;
	memcpy(&result, &b, 4);
	return result;
	}


bytestream::bytestream () {
	pos = 0;
	size = 0;
	stream = nullptr;
	islilEndian = true;
	ownsMemory = true;
	}

bytestream::~bytestream () {
    if (ownsMemory && stream != nullptr) {
        delete[] stream;
        }
	}
size_t bytestream::getFileSizeW(std::wstring fileName) {
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!GetFileAttributesExW((wchar_t*)fileName.c_str(), GetFileExInfoStandard, &fad))
		return -1; // error condition, could call GetLastError to find out more
	LARGE_INTEGER size;
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;
	return (size_t)size.QuadPart;
	}
bool bytestream::doesFileExist (std::string fileName) {
	DWORD dwAttrib = GetFileAttributesA(fileName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}
bool bytestream::doesFileExistW (std::wstring fileName) {
	DWORD dwAttrib = GetFileAttributesW((wchar_t*)fileName.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}
bool bytestream::doesFolderExistW (std::wstring dirName_in) {
	//DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	DWORD ftyp = GetFileAttributesW((wchar_t*)dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;  //something is wrong with your path!
		}
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
		return true;   // this is a directory!
		}
	return false;    // this is not a directory!
	}
bool bytestream::makeDirW(std::wstring wsPath) {
	// https://www.codeguru.com/cpp/w-p/files/article.php/c4439/CreatePath--Full-Path-Creation-wstring-version.htm
	unsigned long attr;
	size_t pos;
	bool result = false;
	std::wstring tmp;

	if (wsPath.length() > 0) {
		if (wsPath[wsPath.length() - 1] != '\\') {
			wsPath += L"\\";
			}
		// Look for existing object:
		attr = GetFileAttributesW(wsPath.c_str());
		if (attr == 0xFFFFFFFF) { // doesn't exist yet - create it!
			pos = wsPath.find_last_of(L"\\", wsPath.length() - 2);
			if (pos != std::wstring::npos) {
				// Create parent dirs:
				result = makeDirW(wsPath.substr(0, pos + 1));
				}
			// Create node:
			result = result && CreateDirectoryW(wsPath.c_str(), NULL);
			CreateDirectoryW(wsPath.c_str(), NULL);
			}
		else if (FILE_ATTRIBUTE_DIRECTORY != attr) {
			// object already exists, but is not a dir
			result = false;
			}
		}
	return result;
	}
std::wstring bytestream::getFilenamePathW (const std::wstring &str) {
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
std::wstring bytestream::getFilenameFileW(const std::wstring &str) {
	// getFilenameFile file   -- returns: "myImage"
	size_t found;
	std::wstring strt;
	found = str.find_last_of(L"/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find(L".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
			}
		//return strt;
		}
	else {
		strt = str;
		//return str;
		}
	size_t lastdot = strt.find_last_of(L".");
	if (lastdot == std::string::npos) return strt;
	return strt.substr(0, lastdot);
	}
std::wstring bytestream::getFilenameTypeW (std::wstring const &path ) {
	// getFilenameType file -- returns: ".jpg"
	std::wstring ext;
	size_t dotIdx = path.find_last_of( L"." );
	if ( dotIdx != std::wstring::npos ) {
		size_t dirSepIdx = path.find_last_of( L"/\\" );
		if ( dotIdx > dirSepIdx + 1 ) {
			ext = path.substr( dotIdx );
			}
		}
	return ext;
	}
std::string bytestream::getFilenamePath (const std::string &str) {
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
std::string bytestream::getFilenameFile(const std::string &str) {
	// getFilenameFile file   -- returns: "myImage"
	unsigned int found = 0;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find(".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
			}
		//return strt;
		}
	else {
		strt = str;
		//return str;
		}
	unsigned int lastdot = strt.find_last_of(".");
	if (lastdot == std::string::npos) return strt;
	return strt.substr(0, lastdot);
	}
std::string bytestream::getFilenameType (std::string const &path ) {
	// getFilenameType file -- returns: ".jpg"
	std::string ext;
	unsigned int dotIdx = path.find_last_of( "." );
	if ( dotIdx != std::string::npos ) {
		unsigned int dirSepIdx = path.find_last_of( "/\\" );
		if ( dotIdx > dirSepIdx + 1 ) {
			ext = path.substr( dotIdx );
			}
		}
	return ext;
	}
void bytestream::setBigEndian () {islilEndian = false;}
void bytestream::setLittleEndian () {islilEndian = true;}
void bytestream::close () {
    if (ownsMemory && stream != nullptr) {delete[] stream;}
    pos = 0;
    size = 0;
    stream = nullptr;
    islilEndian = true;
    ownsMemory = true;
	}
bool bytestream::openFile(std::string filename, size_t file_size) {
	bool result = openFileW(std::wstring(filename.begin(), filename.end()), file_size);
	if (result) {
		// extract components of file path
		file = filename;
		fpath = getFilenamePath(filename);
		fname = getFilenameFile(filename);
		fext = getFilenameType(filename);
		}
	return result;
	}
bool bytestream::openFileW (std::wstring filename, size_t file_size) {
	HANDLE hFile = CreateFileW(
		filename.c_str(),		// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL					// no attr. template
		);

	// check that file was opened successfully
	if (hFile == INVALID_HANDLE_VALUE) {
		//std::wcout << L"Failed to Open File {" << filename << L"}\n";
		CloseHandle(hFile);
		return false;
		}

	// Clear old buffer
	close();

	// get / check file size is larger then 0
	size = GetFileSize(hFile, NULL);
	if (file_size > 0 && file_size < size) {
		size = file_size;
		}

	if (size == 0) {
		//std::cout << "Failed to read file. File is Empty?\n";
		CloseHandle(hFile);
		return false;
		}

	// size char for buffer
	stream = new char[size];

	// attempt to read file contents into memory
	unsigned long dwBytesRead = 0;
	if(ReadFile(hFile, stream, size, &dwBytesRead, NULL) == FALSE || dwBytesRead != size) {
		//std::cout << "Failed to copy file into memory\n";
		CloseHandle(hFile);
		close();
		return false;
		}

	// close out handle :-)
	CloseHandle(hFile);

	// extract components of file path
	fileW = filename;
	fpathW = getFilenamePathW(filename);
	fnameW = getFilenameFileW(filename);
	fextW = getFilenameTypeW(filename);

	return true;
	}

bool bytestream::openResource (int resource_id, const char* resource_class) {

	/*
	https://mklimenko.github.io/english/2018/06/23/embed-resources-msvc/
	https://stackoverflow.com/questions/16527973/load-resource-as-byte-array-programmaticaly-in-c
	usage:
	openResource(IDR_DATA1, "RCDATA");
	*/

	HRSRC hResource = FindResourceA(NULL, MAKEINTRESOURCEA(resource_id), resource_class);
	if (hResource == NULL) {
		std::cout << "failed To Find Resource" << std::endl;
		return false;
		}

	HGLOBAL hMemory = LoadResource(NULL, hResource);
	if (hMemory == NULL) {
		std::cout << "failed To Load Resource" << std::endl;
		return false;
		}

	DWORD dwSize = SizeofResource(NULL, hResource);
	LPVOID lpAddress = LockResource(hMemory);

	if (dwSize == 0) {
		std::cout << "file is empty" << std::endl;
		return false;
		}

	if (stream != NULL) {
		delete[] stream;
		}

	size = dwSize;
	stream = new char[dwSize];
	memcpy(stream, lpAddress, dwSize);
	return true;
	}

bool bytestream::amendFile (std::wstring filename, size_t &offset) {

	HANDLE hFile = CreateFileW (
		filename.c_str(),		// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL					// no attr. template
		);

	// check that file was opened successfully
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to Open File\n";
		return false;
		}
	//std::cout << offset << std::endl;
	// get / check file size is larger then 0
	size_t fsize = GetFileSize(hFile, NULL);
	if (fsize == 0 || offset + fsize > size) {
		std::cout << "Failed to read file. File is Empty?\n";
		return false;
		}

	// attempt to read file contents into memory
	unsigned long dwBytesRead = 0;
	if(ReadFile(hFile, &stream[offset], fsize, &dwBytesRead, NULL) == FALSE || dwBytesRead != fsize) {
		std::cout << "Failed to copy file into memory\n";
		CloseHandle(hFile);
		return false;
		}

	offset += fsize;

	// close out handle :-)
	CloseHandle(hFile);

	return true;
	}
bool bytestream::writeFile (std::string filename, size_t offset, size_t wsize, char* data ) {
	//std::wcout << "filename:\t" << filename << std::endl;
	if (data == NULL) {
		if (wsize < 1) {wsize = size - offset;}
		if (offset + wsize > size) {wsize = size - offset;}
		}
	else if (wsize == 0) {
		return false;
		}

	unsigned long dwBytesWritten = 0;
	unsigned long dwPos = 0;

	//std::wcout << L"File: " << filename << std::endl;
	HANDLE hFile = CreateFileA(
		filename.c_str(),			// open One.txt
		GENERIC_WRITE,						// open for reading
		FILE_SHARE_READ,					// do not share
		NULL,								// no security
		CREATE_ALWAYS,							// existing file only
		FILE_ATTRIBUTE_NORMAL,				// normal file
		NULL								// no attr. template
		);

	if (hFile == INVALID_HANDLE_VALUE) {
		std::cout << L"failed to write file:\n\t" << filename << std::endl;
		return false;
		}

	dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
	LockFile(hFile, dwPos, 0, wsize, 0);
	if (data == NULL) {
		WriteFile(hFile, stream + offset, wsize, &dwBytesWritten, NULL);
		}
	else {
		WriteFile(hFile, data, wsize, &dwBytesWritten, NULL);
		}

	UnlockFile(hFile, dwPos, 0, wsize, 0);
	CloseHandle(hFile);
	return true;
	}
bool bytestream::writeFileW (std::wstring filename, size_t offset, size_t wsize, char* data ) {

    // Handle cases where data is NULL
    if (data == NULL) {
        if (wsize < 1) { wsize = size - offset; }
        if (offset + wsize > size) { wsize = size - offset; }
        }
    else if (wsize == 0) {
        std::wcout << L"Write size is zero. Aborting write operation." << std::endl;
        return false;
        }

    HANDLE hFile = CreateFileW(
        filename.c_str(),    // file name
        GENERIC_WRITE,       // open for writing
        FILE_SHARE_READ,     // share for reading
        NULL,                // no security
        CREATE_ALWAYS,       // create new or overwrite
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL                 // no template
        );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcout << L"Failed to open file for writing: " << filename << std::endl;
        return false;
        }

    // No need to set file pointer to end for new/overwritten files
    // Lock file for writing
    if (!LockFile(hFile, 0, 0, wsize, 0)) {
        std::wcout << L"Failed to lock file for writing: " << filename << std::endl;
        CloseHandle(hFile);
        return false;
        }

    DWORD dwBytesWritten = 0;
    BOOL writeResult;
    if (data == NULL) {
        writeResult = WriteFile(hFile, stream + offset, wsize, &dwBytesWritten, NULL);
        }
    else {
        writeResult = WriteFile(hFile, data, wsize, &dwBytesWritten, NULL);
        }

    // Unlock file after writing
    UnlockFile(hFile, 0, 0, wsize, 0);

    // Close the file handle
    CloseHandle(hFile);

    if (!writeResult || dwBytesWritten != wsize) {
        std::wcout << L"Failed to write the expected number of bytes to file: " << filename << std::endl;
        return false;
        }

    return true;
    }


//bool bytestream::size () {return size;}

bool bytestream::eos () {
	if (pos >= size) {
		return true;
		}
	else {
		return false;
		}
	}
bool bytestream::createFile (unsigned long bufferSize) {
	if (bufferSize > 0) {
		close();
		size = bufferSize;
		stream = new char[bufferSize];
		return true;
		}
	return false;
	}

void bytestream::resize(size_t newsize, bool flush) {
    try {
        if (newsize == size) {
            // No change in size, nothing to do.
            return;
        }

        char* newStream = new char[newsize];
        if (!newStream) {
            throw std::bad_alloc();
        }

        size_t bytesToCopy = 0;
        if (stream != nullptr) {
            // Copy existing data to the new stream
            size_t bytesToCopy = std::min(newsize, static_cast<size_t>(size));

            std::copy(stream, stream + bytesToCopy, newStream);

            // Free old stream
            delete[] stream;
        }

        stream = newStream;
        size = newsize;
        pos = (newsize < size) ? newsize : pos; // Update pos if newsize is smaller

        if (flush) {
            std::fill(stream + bytesToCopy, stream + size, 0); // Zero out the rest
        }
    } catch (const std::bad_alloc& e) {
        std::cerr << "Failed to allocate memory in resize: " << e.what() << std::endl;
        // Handle memory allocation failure
    } catch (const std::exception& e) {
        std::cerr << "Exception in resize: " << e.what() << std::endl;
        // Handle other exceptions
    } catch (...) {
        std::cerr << "Unknown exception in resize" << std::endl;
        // Handle unknown exceptions
    }
}

/** @brief copy's bytes from a char array to the bytestream
 *  @param {src_buf} char array to copy from
 *  @param {src_len} size of the char array coping from
 *  @param {src_pos, Default:0} position in the char array to read from
 *  @param {tar_dest, Default:0} position in bytestream to write to
 *  @return {VOID}
 */
void bytestream::copy (const char* src_buf, size_t src_len, size_t src_pos, size_t tar_dest) {


	// Check Array is Not Empty
	if (src_buf == NULL) {
		std::cout << "Input Buffer is invalid\n";
		return;
		}

	// Check if Resize is needed
	if (tar_dest + src_len > size) {
		resize(tar_dest + src_len);
		}

	// Copy Char Arrays
	std::copy(&src_buf[src_pos], &src_buf[src_pos + src_len], &stream[tar_dest]);
	//for (unsigned int i = 0; i < src_len; i++) {stream[tar_dest + i] = src_buf[src_pos + i];}



	}
void bytestream::assign(char* newStream, size_t newSize) {
    close(); // Ensure the current stream is properly closed
    stream = newStream;
    size = newSize;
    ownsMemory = false; // This object does not own the newStream memory
    }
size_t bytestream::tell () {return pos;}
void bytestream::seek (unsigned long offset, char dir) {
	switch (dir) {
		case 0: pos = offset; break;	// abs
		case 1: pos += offset; break;	// rel
		case 2: pos -= offset; break;	// end
		}
	if (pos > size) {pos = size;}
	}
signed long bytestream::unsigned_to_signed (unsigned long n, char nbits) {	// unsigned_to_signed 0x80 8 = -128
	signed long result = n;
	if (n > pow(2.0f, nbits) / 2.0f) {
		result = (signed long)(n - pow(2.0f, nbits));
		}
	return result;
	}

uint32_t bytestream::crc32 (size_t crc_pos, size_t crc_len) { // Cyclic Redundancy Check

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	// http://www.hackersdelight.org/hdcodetxt/crc.c.txt
	uint32_t crc = 0xFFFFFFFF;
	unsigned int b = 8;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		crc = crc ^ (uint8_t)stream[i];
		for (unsigned int j = 0; j < b; j++) {
			crc = (crc >> 1) ^ (0xEDB88320 & -(signed int)(crc & 1));
			}
		}
	return (crc ^ 0xFFFFFFFF);
	}

uint32_t bytestream::adler32 (size_t crc_pos, size_t crc_len, uint32_t seed) { //  Mark Adler

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	const uint32_t MOD_ADLER = 0xFFF1;
    uint32_t a = seed & 0xFFFF;
    uint32_t b = (seed >> 16) & 0xFFFF;

    for (unsigned int i = crc_pos; i < crc_end; i++) {
        a = (a + (unsigned char)stream[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
		}
    return (b << 16) | a;
	}

uint32_t bytestream::fnv1_32 (size_t crc_pos, size_t crc_len) { //  Fowler Noll Vo

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t FNV_prime = 0x01000193;
	uint32_t hash = 0x811C9DC5;


	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash = hash * FNV_prime;
		hash = hash ^ (unsigned char)stream[i];
		}
	return hash;
	}

uint32_t bytestream::fnv1a_32 (size_t crc_pos, size_t crc_len) { //  Fowler Noll Vo (Variant A)

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t FNV_prime = 0x01000193;
	uint32_t hash = 0x811C9DC5;


	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash = hash ^ (unsigned char)stream[i];
		hash = hash * FNV_prime;
		}
	return hash;
	}

uint32_t bytestream::joaat32 (size_t crc_pos, size_t crc_len) { //  Jenkins One At A Time
	// https://en.wikipedia.org/wiki/Jenkins_hash_function
	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = 0;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash += (unsigned char)stream[i];
		hash += hash << 10;
		hash ^= hash >> 6;
		}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
	}

uint32_t bytestream::djb2_32 (size_t crc_pos, size_t crc_len) { //  Daniel Julius Bernstein

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = 0x00001505;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash = (hash * 33) + (unsigned char)stream[i];
		}
	return hash;
	}

uint32_t bytestream::djb2_32a (size_t crc_pos, size_t crc_len) { //  Daniel Julius Bernstein (Variant A)

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = 0x00001505;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		//hash = bit.xor ((bit.shift hash 5) + hash) (bit.CharAsInt str[i])
		hash = (hash * 33) ^ (unsigned char)stream[i];
		}
	return hash;
	}

uint32_t bytestream::sdbm_32 (size_t crc_pos, size_t crc_len) { //   Small Data Base Manager

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = 0x00001505;
	for (unsigned int i = crc_pos; i < crc_end; i++) {

		hash = (unsigned char)stream[i] + (hash << 6) + (hash << 16) - hash;
		//hash = hash * 65599 + (bit.CharAsInt lstr[i])

		}
	return hash;
	}

uint32_t bytestream::aod_32 (size_t crc_pos, size_t crc_len) { //   Angel of Darkness Hash Algo

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = crc_len;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash ^= (hash >> 2) + 32 * hash + (unsigned char)stream[i];
		}
	return hash;
	}

uint32_t bytestream::fcs16 (size_t crc_pos, size_t crc_len) { //   Frame Check Sequence 16Bit

	if (crc_len == 0) {crc_len = size - crc_pos;}
	size_t crc_end = crc_pos + crc_len;

	uint32_t hash = 0xFFFF;
	for (unsigned int i = crc_pos; i < crc_end; i++) {
		hash = fcs16_lut[(((unsigned char)stream[i] ^ hash) & 0xFF)] ^ (hash >> 8);
		}
	hash ^= 0xFFFF;
	return hash;
	}

std::string bytestream::md5 (size_t crc_pos, size_t crc_len) {

	if (crc_len == 0) {crc_len = size - crc_pos;}
	//size_t crc_end = crc_pos + crc_len;

	unsigned char SHIFT_AMTS[16] = {7, 12, 17, 22, 5,  9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
	unsigned long TABLE_T[64] = { // 64 Entries, 256 Bytes
		0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
		0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
		0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
		0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
		0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
		0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
		0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
		0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
		0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
		0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
		0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
		0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
		0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
		0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
		0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
		0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
		};
	unsigned long numBlocks = ((crc_len + 8) >> 6) + 1;
	unsigned long totalen = numBlocks << 6;
	unsigned long* paddingBytes = nullptr;
	unsigned long paddingen = totalen - crc_len;
	if (paddingen > 0) {
        paddingBytes = new unsigned long[paddingen];
        paddingBytes[0] = 0x80;
        for (unsigned long i = 1; i < paddingen; i++) {
            paddingBytes[i] = 0;
            }
        }
	unsigned long streamenBits = crc_len << 3;
	for (unsigned char i = 0; i < 8; i++) {
		paddingBytes[paddingen - 8 + i] = streamenBits;
		streamenBits = streamenBits >> 8;
		}
	unsigned long INIT_A = 0x67452301;
	unsigned long INIT_B = 0xEFCDAB89;
	unsigned long INIT_C = 0x98BADCFE;
	unsigned long INIT_D = 0x10325476;
	unsigned long a = INIT_A;
	unsigned long b = INIT_B;
	unsigned long c = INIT_C;
	unsigned long d = INIT_D;
	unsigned long f = 0;
	unsigned long buffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16
	unsigned long index = 0;
	unsigned long originalA = INIT_A;
	unsigned long originalB = INIT_B;
	unsigned long originalC = INIT_C;
	unsigned long originalD = INIT_D;
	unsigned char div16 = 0;
	unsigned long bufferIndex = 0;
	unsigned long temp = 0;
	unsigned long temp2 = 0;
	for (unsigned long i = 0; i < numBlocks; i++) {
		index = i << 6;
		for (unsigned char j = 0; j < 64; j++) {
			buffer[j >> 2] = ((index < crc_len ? stream[crc_pos + index] : paddingBytes[index - crc_len]) << 24) | (buffer[j >> 2] >> 8);
			index += 1;
			}
		originalA = a;
		originalB = b;
		originalC = c;
		originalD = d;
		for (unsigned char j = 0; j < 64; j++) {
			div16 = j >> 4;
			f = 0;
			bufferIndex = j;
			switch (div16) {
				case 0: {
				    f = (b & c) | (~b & d);
                    break;
                    }
				case 1: {
					f = (b & d) | (c & ~d);
					bufferIndex = ((bufferIndex * 5) + 1) & 0x0F;
                    break;
                    }
				case 2: {
					f = (b ^ c) ^ d;
					bufferIndex = ((bufferIndex * 3) + 5) & 0x0F;
                    break;
                    }
				case 3: {
					f = c ^ (b | ~d);
					bufferIndex = (bufferIndex * 7) & 0x0F;
                    break;
                    }
				}
			temp2 = SHIFT_AMTS[(div16 << 2) | (j & 3)];
			temp = a + f + buffer[bufferIndex] + TABLE_T[j];
			temp = b + ((temp << temp2) | ((temp >> (32 - temp2)) & ((1 << temp2) - 1)));
			a = d;
			d = c;
			c = b;
			b = temp;
			}
		a += originalA;
		b += originalB;
		c += originalC;
		d += originalD;
		}
    if (paddingBytes != nullptr) {delete[] paddingBytes;}
	unsigned long result[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16
	unsigned char count = 0;
	unsigned long n = 0;
	for (unsigned char i = 0; i < 4; i++) {
        switch (i) {
            case 0: n = a; break;
            case 1: n = b; break;
            case 2: n = c; break;
            case 3: n = d; break;
            }
		for (unsigned char j = 0; j < 4; j++) {
			result[count++] = n & 0xFF;
			n = n >> 8;
			}
		}
	std::string md5str = "";
	for (unsigned char i = 0; i < 16; i++) {
        md5str += n2hexstr(result[i], 2);
		}
    std::cout << "RESULT: \t" << md5str << std::endl;
	return md5str;
	}


std::string bytestream::base64_encode (size_t crc_pos, size_t crc_len) {

	if (crc_len == 0) {crc_len = size - crc_pos;}

	const std::string chset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned int nblocks = crc_len / 3;
	unsigned int p = crc_pos;
	std::string str = "";
	if (crc_len > 0) {

		// calculate size
		// bit.and ((4.0 * 8 / 3.0) + 3.0) (bit.not 3)
		// 4.0 * (Ceil (8 / 3.0))
		unsigned int len = ((4 * crc_len / 3) + 3) & ~3;
		str = std::string (len, '=');

		uint32_t d = 0;
		unsigned int x = 0;
		for (unsigned int i = 0; i < nblocks; i++) {
			d = (unsigned char)stream[p + 2] + ((unsigned char)stream[p + 1] << 8) + ((unsigned char)stream[p + 0] << 16);
			for (unsigned int b = 4; b-- > 0; ) {
				str[x++] = chset[(d >> (6 * b)) & 0x3F];
				}
			p+=3;
			}
		unsigned char pad = 0;
		switch (crc_len - (nblocks * 3)) {
			case 1: {
				d = ((unsigned char)stream[p + 0] << 16);
				p+=1;
				pad = 2;
				break;
				}
			case 2: {
				d = ((unsigned char)stream[p + 1] << 8) + ((unsigned char)stream[p + 0] << 16);
				p+=2;
				pad = 1;
				break;
				}
			}
		if (pad > 0) {
			for (unsigned int b = 4; b-- > pad; ) {str[x++] = chset[(d >> (6 * b)) & 0x3F];}
			//for (unsigned char i = 0; i < pad; i++) {str[x++] = '=';}
			}
		}

	return str;
	}

void bytestream::base64_decode (std::string &input, size_t tar_pos) {
	size_t out_len = 0;
	if (input.length() > 0) {
		out_len = (input.length() / 4) * 3;
		for (unsigned char i = 0; i < 2; i++) {
			if (input.substr(input.length() - (i + 1), 1) == "=") {
				out_len -= 1;
				}
			}
		}
	if (out_len > 0) {
		if (tar_pos + out_len > size) {
			resize(tar_pos + out_len);
			}
		const std::string chset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		unsigned int w1, w2, w3, w4;
		unsigned int count = input.length() / 4;
		for (unsigned int i = 0; i < count; i++) {
			w1 = chset.find(input.substr((i * 4) + 0, 1));
			w2 = chset.find(input.substr((i * 4) + 1, 1));
			w3 = chset.find(input.substr((i * 4) + 2, 1));
			w4 = chset.find(input.substr((i * 4) + 3, 1));
			if (w2 != std::string::npos) {stream[tar_pos++] = (unsigned char)(w1 *  4 + (w2 / 16)) & 0xFF;}
			if (w3 != std::string::npos) {stream[tar_pos++] = (unsigned char)(w2 * 16 + (w3 /  4)) & 0xFF;}
			if (w4 != std::string::npos) {stream[tar_pos++] = (unsigned char)(w3 * 64 +  w4      ) & 0xFF;}
			}
		}
	}

void bytestream::apply_xor_key (const char* key, size_t keylen, size_t tar_pos, size_t tar_size) {

    if (keylen <= 0) {
        std::cout << "key is empty\n";
        return;
        }

    if (tar_pos + tar_size > size) {
        std::cout << "tar_pos is out of bounds of file stream\n";
        return;
        }

    if (tar_size <= 0) {
        tar_size = size - tar_pos;
        }
    size_t endpos = tar_size + tar_pos;
    unsigned int k = 0;
    for (unsigned int i = tar_pos; i < endpos; i++) {
        if (k >= keylen) {k = 0;}
        //std::cout << (int)stream[i] << " : \t" << ((int)stream[i] ^ (int)key[++k]) << std::endl;
        //stream[i] = (char)((int)stream[i] ^ (int)key[++k]);
        stream[i] = stream[i] ^ key[k++];
        }
    }

void bytestream::asPNG (char* &buf, unsigned int width, unsigned int height, unsigned char channels, bool flipV, bool reverse) {

    /*
        saves a raw pixel buffer into a uncompressed PNG
    */
    std::string datastr = "Sat 15 Apr 2023 10:46:20 -0500";
	int year = 2023;
	unsigned char month = 1;
	unsigned char day = 1;
	unsigned char hour = 0;
	unsigned char min = 0;
	unsigned char sec = 0;
	float gamma = 1.0f / 2.2f;
	unsigned int image_size = width * height * channels;
	unsigned int data_size = image_size + height;
    unsigned int width_stride = width * channels;
	const uint32_t MOD_ADLER = 0xFFF1;
    uint32_t a = 1 & 0xFFFF;
    uint32_t b = (1 >> 16) & 0xFFFF;

    unsigned int w = 0;


    //for (unsigned int i = 0; i < image_size; i++) {buf[i] = 255 * std::pow((unsigned char)buf[i]/255.0, 1.0);}

    if (flipV && channels > 0) {
        /*
            frustratingly I could not figure this out on my own
            the swapping code is copied from chatGPT.
        */
        int leftIndex;
        int rightIndex;
        for (unsigned int y = 0; y < height; y++) {
            for (unsigned int x = 0; x < width / 2; x++) {
                leftIndex = (y * width + x) * channels;
                rightIndex = (y * width + (width - 1 - x)) * channels;
                std::swap_ranges(buf + leftIndex, buf + leftIndex + channels, buf + rightIndex);
                }
            }
        }


    unsigned int p = 0;
    if (reverse) {p = image_size;}
    for (unsigned int i = 0; i < image_size; i++) {
        if (i % width_stride == 0) { // header for each row of pixels
            a = a % MOD_ADLER;
            b = (b + a) % MOD_ADLER;
            }
        if (reverse) {a = (a + (unsigned char)buf[--p]) % MOD_ADLER;}
        else {a = (a + (unsigned char)buf[p++]) % MOD_ADLER;}

        b = (b + a) % MOD_ADLER;
		}
    uint32_t ADLER = (b << 16) | a;

    /*
        theres alot of fluff to write in order to write
        just a raw array of pixels as a PNG...

        seems like a chunk format, chunk format as follows

        uint32_t data_size // size of data only, 12 bytes for crc, chunk type and size excluded
        uint32_t chunk_type
        uint8_t[] data
        uint32_t chunk_crc // excludes the data_size

        ok found out that after I uncompress the datastream in the PNG
        the data itself is compressed using 4 methods.

        I think I can use method 0 which inserts a null in front of
        each row of pixels :/


    */

    //unsigned int c = 0;
    unsigned int chunk_size = 0xFFFF;
    unsigned int num_chunks = (unsigned int)std::ceil((float)data_size / (float)chunk_size);
    unsigned int chunk_remain = data_size - (((unsigned int)std::floor((float)data_size / (float)chunk_size)) * chunk_size);
    size_t outsize = 145 + datastr.size() + data_size + (num_chunks * 5);

    // create char array to write to

//    std::cout << "data_size: \t" << data_size << std::endl;
//    std::cout << "OutputSize: \t" << outsize << std::endl;
//    std::cout << "chunk_size: \t" << chunk_size << std::endl;
//    std::cout << "num_chunks: \t" << num_chunks << std::endl;
//    std::cout << "chunk_remain: \t" << chunk_remain << std::endl;
    resize(outsize);
    seek(0);
    setBigEndian();

    // Write PNG Start Block (8 Bytes)
    writeUlong(0x89504E47); // PNG
    writeUlong(0x0D0A1A0A); // Version?

    // Write IHDR {Image Info} Block (25 Bytes)
    writeUlong(0x0000000D); // Info Size
    writeUlong(0x49484452); // Info Type
    writeUlong(width); // Width
    writeUlong(height); // Length
    writeUbyte(8); // Bits Per Channel: 8
    switch (channels) { // Color type: RGB
        case 3: {writeUbyte(2); break;}
        case 4: {writeUbyte(6); break;}
        }
    writeUbyte(0); // Compression method: DEFLATE
    writeUbyte(0); // Filter method: Adaptive
    writeUbyte(0); // Interlace method: None
    writeUlong(crc32(12, 17)); // CRC1 goes here

    // Write Text Block (26 + n Bytes)
    writeUlong(0x0000002C); // Text Size
    writeUlong(0x74584574); // Text Type
    writestring("Creation Time");
    writestring(datastr, datastr.size()); // lacks a 0 terminator
    writeUlong(crc32(37, 18 + datastr.size())); // CRC2 goes here

    // Write Date Block (19 Bytes)
    writeUlong(0x00000007); // Date Size
    writeUlong(0x74494D45); // Date Type
    writeUshort(year); // Year
    writeUbyte(month); // Month
    writeUbyte(day); // Day
    writeUbyte(hour); // Hour
    writeUbyte(min); // Minute
    writeUbyte(sec); // Second
    writeUlong(crc32(63 + datastr.size(), 11)); // CRC3 goes here

    // Write Print Block (21 Bytes)
    writeUlong(0x00000009); // Print Size
    writeUlong(0x70485973); // Print Type
    writeUlong(0x00000B11); // 2833 X pixels per unit of 72 Dots Per Inch
    writeUlong(0x00000B11); // 2833 Y pixels per unit of 72 Dots Per Inch
    writeUbyte(1); // Units: 1 Metre
    writeUlong(crc32(82 + datastr.size(), 13)); // CRC4 goes here

    // Write Gamma Block (16 Bytes)
    writeUlong(0x00000004); // Gamma Size
    writeUlong(0x67414D41); // Gamma Type
    writeUlong((unsigned int)(gamma * 100000.0f)); // Gamma: 0.45455
    writeUlong(crc32(103 + datastr.size(), 8)); // CRC5 goes here

    // Write Image Data Block (18 + n Bytes) n = zlib compressed stream
    writeUlong((num_chunks * 5) + data_size + 6); // Data Size
    writeUlong(0x49444154); // Data Type
    writeUbyte(0x78); // compression method format
    writeUbyte(0x01); // compression flag: No Compression
    setLittleEndian();
    width_stride += 1;
    p = 0;
    if (reverse) {p = image_size;}
    w = 0;
    for (unsigned int c = 0; c < num_chunks; c++) {


        // Write Data in blocks

        if (c + 1 < num_chunks) {

            // Write Type As RAW / Uncompressed
            writeUbyte(0x00); // flag to stop reading

            writeUshort(chunk_size);
            writeUshort(0);

            for (unsigned int i = 0; i < chunk_size; i++) {

                if (w % width_stride == 0) {writeUbyte(0);} // header for each row of pixels
                else {if (reverse) {writeUbyte(buf[--p]);}else{writeUbyte(buf[p++]);}}
                w++;
                }
            }
        else {

            // Write Type As RAW / Uncompressed
            writeUbyte(0x01);

            writeUshort(chunk_remain);
            writeUshort(0xFFFF - chunk_remain);
            for (unsigned int i = 0; i < chunk_remain; i++) {

                if (w % width_stride == 0) {writeUbyte(0);} // header for each row of pixels
                else {if (reverse) {writeUbyte(buf[--p]);}else{writeUbyte(buf[p++]);}}
                w++;
                }
            }
        }
    setBigEndian();
//    std::cout << "Pointer: \t" << p << std::endl;
    writeUlong(ADLER); // CRC6 goes here {is the decompressed chechsum as adler32}

    writeUlong(crc32(119 + datastr.size(), (num_chunks * 5) + data_size + 10)); // CRC7 goes here

//    std::cout << "CRC Pos: \t" << 119 + datastr.size() << std::endl;
//    std::cout << "CRC Size: \t" << (num_chunks * 5) + data_size + 14 << std::endl;



    // Write PNG End Block (12 bytes)
    writeUlong(0x00000000); // End Size
    writeUlong(0x49454E44); // End Type
    writeUlong(0xAE426082); // CRC8 goes here

    // for testing
    // save
    //writeFile("E:\\BackUp\\MyCloud4100\\Coding\\Cpp\\Projects\\imv\\test.png");

    // clean up char array
    //close();
    }


void bytestream::asDDS(char* &buf, unsigned int width, unsigned int height, unsigned int mipCount, std::string type) {
    // Calculate the DDS Image Size
//    std::cout << "width: \t" << width << std::endl;
//    std::cout << "height: \t" << height << std::endl;
//    std::cout << "mipCount: \t" << mipCount << std::endl;
//    std::cout << "type: \t" << type << std::endl;
    uint32_t blockSize = 16; // Block size in bytes for DXT formats
    bool isDXT = true;
    if (type == "ARGB") {
        blockSize = 4; // 4 bytes per pixel for ARGB
        isDXT = false;
        }
    else if (type == "DXT1") {
        blockSize = 8;
        }

    uint32_t totalSize = 128; // Start with the size of the header
    if (mipCount == 0) { mipCount = 1; }
    for (uint32_t level = 0; level < mipCount; ++level) {
        uint32_t levelWidth = std::max(1u, width >> level);
        uint32_t levelHeight = std::max(1u, height >> level);
        if (isDXT) {
            uint32_t rowBlocks = (levelWidth + 3) / 4;
            uint32_t columnBlocks = (levelHeight + 3) / 4;
            totalSize += rowBlocks * columnBlocks * blockSize;
            }
        else {
            totalSize += levelWidth * levelHeight * blockSize;
            }
        }
//    std::cout << totalSize<< std::endl;

    resize(totalSize);




    seek(0);
    setLittleEndian();
    // Initialize the header with 0s
    std::memset(stream, 0, 128);
    // Write DDS magic number
    *reinterpret_cast<uint32_t*>(stream) = 0x20534444;
    // Write header size after the magic number
    *reinterpret_cast<uint32_t*>(stream + 4) = 124;
    // Write flags with mip map count if mip maps are present
    *reinterpret_cast<uint32_t*>(stream + 8) = 0x000A1007 | (mipCount > 1 ? 0x20000 : 0);
    // Write height and width
    *reinterpret_cast<uint32_t*>(stream + 12) = height;
    *reinterpret_cast<uint32_t*>(stream + 16) = width;
    // Compute and write linear size or pitch
    if (isDXT) {
        *reinterpret_cast<uint32_t*>(stream + 20) = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        }
    else {
        *reinterpret_cast<uint32_t*>(stream + 20) = (width * height) * 4;//(width * 32 + 7) / 8; // Pitch for uncompressed format
        *reinterpret_cast<uint32_t*>(stream + 80) |= 0x8; // Set the pitch flag for uncompressed images
        }
    // Write mipMapCount
    *reinterpret_cast<uint32_t*>(stream + 28) = mipCount;
    // Set pixel format size
    *reinterpret_cast<uint32_t*>(stream + 76) = 32; // The size of DDS_PIXELFORMAT structure
    // Set pixel format flags and FOURCC
    if (isDXT) {
        // Set flags for DXT formats
        *reinterpret_cast<uint32_t*>(stream + 80) = 0x00000004;
        // Set the FOURCC value based on the type
        uint32_t fourCC = 0;
        if (type == "DXT1") {
            fourCC = 0x31545844;
            }
        else if (type == "DXT3") {
            fourCC = 0x33545844;
            }
        else if (type == "DXT5") {
            fourCC = 0x35545844;
            }
        *reinterpret_cast<uint32_t*>(stream + 84) = fourCC;
        }
    else {
        // Set flags for ARGB format
        *reinterpret_cast<uint32_t*>(stream + 80) = 0x40 | 0x1; // DDPF_RGB | DDPF_ALPHAPIXELS
        // The ARGB format does not use FOURCC, set it to 0
        *reinterpret_cast<uint32_t*>(stream + 84) = 0;
        // Set the RGB bit count to 32 bits per pixel
        *reinterpret_cast<uint32_t*>(stream + 88) = 32;
        // Set the R, G, B, and A masks for ARGB
        *reinterpret_cast<uint32_t*>(stream + 92) = 0x00FF0000; // Red mask
        *reinterpret_cast<uint32_t*>(stream + 96) = 0x0000FF00; // Green mask
        *reinterpret_cast<uint32_t*>(stream + 100) = 0x000000FF; // Blue mask
        *reinterpret_cast<uint32_t*>(stream + 104) = 0xFF000000; // Alpha mask
        }
    // Write caps
    *reinterpret_cast<uint32_t*>(stream + 108) = 0x00401008;
    // Copy the image data from buf to the stream, after the header
    std::memcpy(stream + 128, buf, totalSize - 128); // Copy the image data after the header
    }


uint8_t bytestream::readUbyte () {
	uint8_t result = 0xFF;
	if (pos + 1 <= size) {
		result = (uint8_t)stream[pos];
		pos += 1;
		}
	return result;
	}
uint16_t bytestream::readUshort () {
	unsigned char byteOrder[] = {0, 1};
	if (!islilEndian) {byteOrder[0] = 1; byteOrder[1] = 0;}
	uint16_t result = 0xFFFF;
	if (pos + 2 <= size) {
		//result = (uint16_t)((uint8_t)stream[pos + byteOrder[0]] * 0x0001);
		//result += (uint16_t)((uint8_t)stream[pos + byteOrder[1]] * 0x0100);
		result = ((unsigned short)stream[pos + byteOrder[1]] << 8) | (unsigned char)stream[pos + byteOrder[0]];
		pos += 2;
		}
	return result;
	}
uint32_t bytestream::readUlong () {
	unsigned char byteOrder[] = {0, 1, 2, 3};
	if (!islilEndian) {
		byteOrder[0] = 3; byteOrder[1] = 2;
		byteOrder[2] = 1; byteOrder[3] = 0;
		}
	uint32_t result = 0xFFFFFFFF;
	if (pos + 4 <= size) {
		result = (uint32_t)((uint8_t)stream[pos + byteOrder[0]] * 0x00000001);
		result += (uint32_t)((uint8_t)stream[pos + byteOrder[1]] * 0x00000100);
		result += (uint32_t)((uint8_t)stream[pos + byteOrder[2]] * 0x00010000);
		result += (uint32_t)((uint8_t)stream[pos + byteOrder[3]] * 0x01000000);



/*
int a = int((unsigned char)(buffer[0]) << 24 |
            (unsigned char)(buffer[1]) << 16 |
            (unsigned char)(buffer[2]) << 8 |
            (unsigned char)(buffer[3]));
*/



		pos += 4;
		}
	return result;
	}
uint64_t bytestream::readUlonglong() {
    unsigned char byteOrder[] = {0, 1, 2, 3, 4, 5, 6, 7};
    if (!islilEndian) {
        byteOrder[0] = 7; byteOrder[1] = 6; byteOrder[2] = 5;
        byteOrder[3] = 4; byteOrder[4] = 3; byteOrder[5] = 2;
        byteOrder[6] = 1; byteOrder[7] = 0;
    }

    uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= static_cast<uint64_t>(static_cast<uint8_t>(stream[pos + byteOrder[i]])) << (8 * i);
    }

    pos += 8;
    return result;
}

int8_t bytestream::readbyte() {return (int8_t)unsigned_to_signed(readUbyte(), 8);}
int16_t bytestream::readshort() {return (int16_t)unsigned_to_signed(readUshort(), 16);}
int32_t bytestream::readlong() {return (int32_t)unsigned_to_signed(readUlong(), 32);}
int64_t bytestream::readlonglong() {return (int64_t)unsigned_to_signed((unsigned long)readUlonglong(), 64);}
std::string bytestream::readstring (int length, unsigned char term) {
	std::string result = "";
	unsigned char letter;
	bool isTerm = false;
	if (length < 0) {
		while (isTerm == false && pos < size) {
			if (pos < size) {
				letter = stream[pos];
				pos++;
				}
			else {
				isTerm = true;
				}
			if (letter == term) {isTerm = true;}
			if (isTerm == false) {result += letter;}
			}
		}
	else {
		if (length > 0 && (pos + length) <= size) {
			result.reserve(length);
			for (int i = 0; i < length; ++i) {
				letter = stream[pos + i];
				if (letter == 0) {isTerm = true;}
				if (isTerm == false) {result += letter;}
				}
			pos += length;
			}
		}
	return result;
	}
std::wstring bytestream::readstringW (int length, wchar_t term) {
	std::wstring result = L"";
	wchar_t letter;
	bool isTerm = false;
	if (length < 0) {
		while (isTerm == false && pos < size) {
			if (pos < size) {
				letter = readUshort();
				}
			else {
				isTerm = true;
				}
			if (letter == term) {isTerm = true;}
			if (isTerm == false) {
				result += letter;
				}
			}
		}
	else {
		if (length > 0 && (pos + length) <= size) {
			size_t jump = pos + length;
			result.reserve(length);
			for (int i = 0; i < length; ++i) {
				letter = readUshort();
				if (letter == 0) {
					isTerm = true;
					break;
					}
				if (isTerm == false) {result += letter;}
				}
			pos = jump;
			}
		}
	return result;
	}
std::string bytestream::readline () {
	uint32_t startpoint = pos;
	std::string line = "";
	while (pos < size && stream[pos] != 0x0A && stream[pos] != 0x0D) {
		pos++;
		}
	if (pos > startpoint) {
		line = std::string(&stream[startpoint], pos - startpoint);
		}
	while (pos < size && (stream[pos] == 0x0A || stream[pos] == 0x0D)) {
		pos++;
		}
	return line;
	}
std::wstring bytestream::readlineW () {
	std::wstring line = L"";
	uint16_t b;
	while (!eos()) {
		b = readUshort();
		if (b == 0x0A || b == 0x0D) {
			break;
			}
		line += b;
		}
	return line;
	}
float bytestream::readfloat () {
	float result = 0.0;
	unsigned long b = readUlong();
	memcpy(&result, &b, 4);
	return result;
	}
double bytestream::readdouble () {
	double result = 0.0;
	unsigned long long b = readUlonglong();
	memcpy(&result, &b, 8);
	return result;
	}
void bytestream::writebyte (unsigned char val) {
	stream[pos] = val;
	pos++;
	}
void bytestream::writeshort (unsigned short val) {
	if (!islilEndian) {
		stream[pos + 1] = val & 0x00FF;
		stream[pos + 0] = (val & 0xFF00) >> 8;
		}
	else {
		stream[pos + 1] = (val & 0xFF00) >> 8;
		stream[pos + 0] = val & 0x00FF;
		}
	pos+=2;
	}
void bytestream::writelong (unsigned long val) {
	if (!islilEndian) {
		stream[pos + 3] = (uint8_t)(val & 0x00FF);
		stream[pos + 2] = (uint8_t)((val & 0xFF00) >> 0x08);
		stream[pos + 1] = (uint8_t)((val & 0xFF0000) >> 0x10);
		stream[pos + 0] = (uint8_t)((val & 0xFF000000) >> 0x18);
		}
	else {
		stream[pos + 3] = (uint8_t)((val & 0xFF000000) >> 0x18);
		stream[pos + 2] = (uint8_t)((val & 0xFF0000) >> 0x10);
		stream[pos + 1] = (uint8_t)((val & 0xFF00) >> 0x08);
		stream[pos + 0] = (uint8_t)(val & 0x00FF);
		}
	pos+=4;
	}
void bytestream::writeUbyte (unsigned char val) {
	stream[pos] = val;
	pos++;
	}
void bytestream::writeUshort (unsigned short val) {
	if (!islilEndian) {
		stream[pos + 1] = val & 0x00FF;
		stream[pos + 0] = (val & 0xFF00) >> 8;
		}
	else {
		stream[pos + 1] = (val & 0xFF00) >> 8;
		stream[pos + 0] = val & 0x00FF;
		}
	pos+=2;
	}
void bytestream::writeUlong (unsigned long val) {
	if (!islilEndian) {
		stream[pos + 3] = (uint8_t)(val & 0x00FF);
		stream[pos + 2] = (uint8_t)((val & 0xFF00) >> 0x08);
		stream[pos + 1] = (uint8_t)((val & 0xFF0000) >> 0x10);
		stream[pos + 0] = (uint8_t)((val & 0xFF000000) >> 0x18);
		}
	else {
		stream[pos + 3] = (uint8_t)((val & 0xFF000000) >> 0x18);
		stream[pos + 2] = (uint8_t)((val & 0xFF0000) >> 0x10);
		stream[pos + 1] = (uint8_t)((val & 0xFF00) >> 0x08);
		stream[pos + 0] = (uint8_t)(val & 0x00FF);
		}
	pos+=4;
	}
void bytestream::writeUlonglong (unsigned long long val) {
	if (!islilEndian) {
		stream[pos + 7] = (uint8_t)(val & 0x00FF);
		stream[pos + 6] = (uint8_t)((val & 0xFF00) >> 8);
		stream[pos + 5] = (uint8_t)((val & 0xFF0000) >> 16);
		stream[pos + 4] = (uint8_t)((val & 0xFF000000) >> 24);
		stream[pos + 3] = (uint8_t)((val & 0xFF00000000) >> 32);
		stream[pos + 2] = (uint8_t)((val & 0xFF0000000000) >> 40);
		stream[pos + 1] = (uint8_t)((val & 0xFF000000000000) >> 48);
		stream[pos + 0] = (uint8_t)((val & 0xFF00000000000000) >> 56);
		}
	else {
		stream[pos + 7] = (uint8_t)((val & 0xFF00000000000000) >> 56);
		stream[pos + 6] = (uint8_t)((val & 0xFF000000000000) >> 48);
		stream[pos + 5] = (uint8_t)((val & 0xFF0000000000) >> 40);
		stream[pos + 4] = (uint8_t)((val & 0xFF00000000) >> 32);
		stream[pos + 3] = (uint8_t)((val & 0xFF000000) >> 24);
		stream[pos + 2] = (uint8_t)((val & 0xFF0000) >> 16);
		stream[pos + 1] = (uint8_t)((val & 0xFF00) >> 8);
		stream[pos + 0] = (uint8_t)(val & 0x00FF);
		}
	pos+=8;
	}
void bytestream::writelonglong (long long val) {
    if (!islilEndian) {
        stream[pos + 7] = (uint8_t)(val & 0x00FF);
        stream[pos + 6] = (uint8_t)((val & 0xFF00) >> 8);
        stream[pos + 5] = (uint8_t)((val & 0xFF0000) >> 16);
        stream[pos + 4] = (uint8_t)((val & 0xFF000000) >> 24);
        stream[pos + 3] = (uint8_t)((val & 0xFF00000000) >> 32);
        stream[pos + 2] = (uint8_t)((val & 0xFF0000000000) >> 40);
        stream[pos + 1] = (uint8_t)((val & 0xFF000000000000) >> 48);
        stream[pos + 0] = (uint8_t)((val & 0xFF00000000000000) >> 56);
    } else {
        stream[pos + 7] = (uint8_t)((val & 0xFF00000000000000) >> 56);
        stream[pos + 6] = (uint8_t)((val & 0xFF000000000000) >> 48);
        stream[pos + 5] = (uint8_t)((val & 0xFF0000000000) >> 40);
        stream[pos + 4] = (uint8_t)((val & 0xFF00000000) >> 32);
        stream[pos + 3] = (uint8_t)((val & 0xFF000000) >> 24);
        stream[pos + 2] = (uint8_t)((val & 0xFF0000) >> 16);
        stream[pos + 1] = (uint8_t)((val & 0xFF00) >> 8);
        stream[pos + 0] = (uint8_t)(val & 0x00FF);
    }
    pos += 8;
}

//void bytestream::writefloat (float val) {
//	unsigned char* c = (unsigned char*) &val;
//	if (!islilEndian) {
//		stream[pos + 3] = c[0];
//		stream[pos + 2] = c[1];
//		stream[pos + 1] = c[2];
//		stream[pos + 0] = c[3];
//		}
//	else {
//		stream[pos + 3] = c[3];
//		stream[pos + 2] = c[2];
//		stream[pos + 1] = c[1];
//		stream[pos + 0] = c[0];
//		}
//	pos+=4;
//	}
void bytestream::writestring (std::string str, signed long length) {
	if (length > -1) {
		unsigned long len = length;
		for (unsigned long i = 0; i < len; i++) {
			if (i < str.size()) {
				writebyte(str[i]);
				}
			else {
				writebyte(0);
				}
			}
		}
	else {
		if (str.size() > 0) {
			for (unsigned long i = 0; i < str.size(); i++) {
				writebyte(str[i]);
				}
			}
		writebyte(0);
		}
	}
void bytestream::writefloat (float val) {
	unsigned char* c = (unsigned char*) &val;
	unsigned char order[4] = {0, 1, 2, 3};
	if (!islilEndian) {
		order[0] = 3; order[1] = 2;
		order[2] = 1; order[3] = 0;
		}
	for (int i = 0; i < 4; i++) {
		stream[pos + order[i]] = c[i];
		}
	pos+=4;
	}
void bytestream::writedouble (double val) {
	unsigned char* c = (unsigned char*) &val;
	char order[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	for (int i = 0; i < 8; i++) {
		stream[pos + order[i]] = c[i];
		}
	pos+=8;
    }
