//
// Created by Oleg Morozenkov on 25.01.17.
//

#include "tgbot/types/InputFile.h"

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

namespace
{

vector<string> split(const string& str, char delimiter)
{
    vector<string> result;
    istringstream stream(str);
    string s;
    while (getline(stream, s, delimiter))
    {
        result.push_back(s);
    }
    return result;
}

std::string readFile(const std::string& filePath)
{
    ostringstream contents;
    ifstream in(filePath, ios::in | ios::binary);
    if (in)
    {
        contents << in.rdbuf();
    }
    return contents.str();
}

}

namespace TgBot
{

InputFile::Ptr InputFile::fromFile(const string& filePath, const string& mimeType)
{
    InputFile::Ptr result(new InputFile);
    result->data = readFile(filePath);
    result->mimeType = mimeType;
    result->fileName = split(filePath, '/').back();
    return result;
}

}
