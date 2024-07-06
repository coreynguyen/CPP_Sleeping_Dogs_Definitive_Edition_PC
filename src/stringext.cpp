#include "windows.h"
#include "stringext.h"

std::string ltrim (std::string str, const char* t) {
    // https://stackoverflow.com/a/25385766
    str.erase(0, str.find_first_not_of(t));
    return str;
    }

std::string rtrim (std::string str, const char* t) {
    // https://stackoverflow.com/a/25385766
    str.erase(str.find_last_not_of(t) + 1);
    return str;
    }

std::string padString (std::string str, unsigned int len, std::string pad, bool addToLeft) {
	std::string temp = str;
	std::string padding = "";
	if (temp.size() > len) {
		temp = temp.substr(0, len);
		}
	if ((len - temp.size()) > 0) {
		for (unsigned int i = 0; i < ((len - temp.size())); i++){
			padding += pad;
			}
		}
	if (addToLeft) {
		return (padding + temp);
		}
	else {
		return (temp + padding);
		}
	}

std::string toupper(const std::string & s) {
	std::string ret(s.size(), char());
	for(unsigned int i = 0; i < s.size(); ++i) {
		ret[i] = (s[i] <= 'z' && s[i] >= 'a') ? s[i]-('a'-'A') : s[i];
		}
	return ret;
	}

std::string tolower(const std::string &s) {
	std::string ret(s.size(), char());
	for (unsigned int i = 0; i < s.size(); ++i) {
		ret[i] = (s[i] <= 'Z' && s[i] >= 'A') ? s[i]-('A'-'a') : s[i];
		}
	return ret;
	}

std::vector<std::string> split(std::string str, std::string token) {
    // https://stackoverflow.com/a/46943631
    /*
    std::vector<std::string>result;
    while (str.size() ){
        unsigned int index = str.find(token);
        if(index!=std::string::npos) {
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
            }
        else {
            result.push_back(str);
            str = "";
            }
        }
    return result;
    */
    // https://tousu.in/qa/?qa=451507/
	std::vector<std::string> wordVector;
	std::stringstream stringStream(str);
	std::string line;
	while(std::getline(stringStream, line)) {
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(token, prev)) != std::string::npos) {
			if (pos > prev) {
				wordVector.push_back(line.substr(prev, pos-prev));
				}
			prev = pos + 1;
			}
		if (prev < line.length()) {
			wordVector.push_back(line.substr(prev, std::string::npos));
			}
		}
	return wordVector;
    }

std::string trim(const std::string& str, const std::string& whitespace) {
    // https://stackoverflow.com/a/1798170
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
	}

std::string reduce(const std::string& str, const std::string& fill, const std::string& whitespace) {
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos) {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
		}

    return result;
	}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    // https://stackoverflow.com/a/24315631
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
    return str;
    }

std::string get_part_date(const std::string &datepart, time_t now) {
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format

    struct tm  tstruct;
    char       buf[80];
	tstruct = *localtime(&now);
	//time_t     now = time(0); errno_t error = *localtime_s(&tstruct, &now);


    strftime(buf, sizeof(buf), datepart.c_str(), &tstruct);
    return buf;
	}

std::string IntToHexString(int number, int length) {
    std::string s;
    std::ostringstream temp;
    temp << std::hex << number;
    s = toupper(temp.str());
    s.insert(s.begin(), length - s.length(), '0');
    return ("0x" + s);
    }

std::string wstring_to_string (std::wstring ws) {
	// http://blog.mijalko.com/2008/06/convert-stdstring-to-stdwstring.html
	std::string s;
	return s.assign(ws.begin(), ws.end());
	}

std::string separateNumbers (std::string str, bool getNums) {
    std::string nstr = "";
    for (unsigned int i = 0; i < str.length(); i++) {
        if (str[i] > 47 && str[i] < 58) {
            if (getNums) {nstr += str[i];}
            } else if (!getNums) {nstr += str[i];}
        }
    return nstr;
    }

int findString (std::string &searchingIn, std::string findThis) {
    int result = -1;
    unsigned int found;
    if (searchingIn.size() > 0) {
        found = searchingIn.find(findThis);
        if (found > searchingIn.size()) {result = -1;} else {result = found;}
        }
    return result;
    }

// Wide Character Functions

std::wstring toUpperW(const std::wstring &s) {
	std::wstring ret(s.size(), char());
	for (unsigned int i = 0; i < s.size(); ++i) {
		ret[i] = (s[i] <= L'z' && s[i] >= L'a') ? s[i]-(L'a'-L'A') : s[i];
		}
	return ret;
	}

std::wstring toLowerW(const std::wstring &s) {
	std::wstring ret(s.size(), char());
	for (unsigned int i = 0; i < s.size(); ++i) {
		ret[i] = (s[i] <= L'Z' && s[i] >= L'A') ? s[i]-(L'A'-L'a') : s[i];
		}
	return ret;
	}

std::wstring ReplaceAllW(std::wstring str, const std::wstring& from, const std::wstring& to) {
    // https://stackoverflow.com/a/24315631
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
    return str;
    }

std::wstring string_to_wstring (std::string s) {
	// http://blog.mijalko.com/2008/06/convert-stdstring-to-stdwstring.html
	std::wstring ws;
	return ws.assign(s.begin(), s.end());
	}

