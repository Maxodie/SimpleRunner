#include "Utils/FileSystem.hpp"

#ifdef _WIN32
#include "ShlObj_core.h"
#endif

namespace SR
{

bool FileSystem::SyncReadAtPathAsString(const Path& path, std::string& outData)
{
    CORE_ASSERT(!path.empty(), "path is empty");

    File file;
    if(OpenFile(path, file, FileMode::FILE_MODE_READ_BINARY))
    {
        size_t bufferSize = GetFileSize(file);

        outData.resize(bufferSize);

        size_t rBytesRead;
        bool result = SyncReadFileAsByte(file, &outData[0], bufferSize, rBytesRead);
        CloseFile(file);

        if(result)
        {
            return true;
        }
    }

    CORE_LOG_ERROR("Failed to read file as string : %s", path.string().c_str());
    return false;
}

bool FileSystem::SyncWriteAtPathAsString(const Path& path, const std::string& data)
{
    CORE_ASSERT(!path.empty(), "path is empty");

    File file;
    if(OpenFile(path, file, FileMode::FILE_MODE_WRITE_BINARY))
    {
        size_t bufferSize = GetFileSize(file);

        size_t rBytesRead;
        bool result = SyncWriteFileAsByte(file, data.c_str());
        CloseFile(file);

        if(result)
        {
            return true;
        }
    }

    CORE_LOG_ERROR("Failed to write file as string : %s", path.string().c_str());
    return false;
}

bool FileSystem::CreateFolder(const Path& path)
{
    return std::filesystem::create_directory(path);
}

bool FileSystem::CreateFolder(const Path& path, const std::string& folderName)
{
    return std::filesystem::create_directory(path / folderName);
}

bool FileSystem::SyncReadFileAsByte(File& file, char* outBuffer, size_t bufferSize, size_t& outrBytesRead)
{
    CORE_ASSERT(outBuffer != nullptr, "path is nullptr");

    file.handle.read(outBuffer, bufferSize);

    std::streamsize size = file.handle.gcount();

    if(file.handle)
    {
        outrBytesRead = size;
        return true;
    }

    outrBytesRead = 0;
    CORE_LOG_ERROR("Failed to read all byte, read size : %zu, file : %s", size, file.path.c_str());
    return false;
}

bool FileSystem::SyncWriteFileAsByte(File& file, const char* buffer)
{
    file.handle << buffer;
    if(file.handle)
    {
        return true;
    }

    return false;
}

Path FileSystem::GetPersistentDataPath()
{
#ifdef _WIN32
    TCHAR   achDevice[MAX_PATH];
    HRESULT  hr;
    // include file ShlObj.h contains list of CSIDL defines however only a subset
    // are supported with Windows 7 and later.
    // for the 3rd argument, hToken, can be a specified Access Token or SSID for
    // a user other than the current user. Using NULL gives us the current user.
    if (SUCCEEDED(hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, achDevice))) {
        // append a folder name to the user's Documents directory.
        // the Path Handling functions are pretty handy.
        /*PathAppend(achDevice, L"xxx");*/
        return achDevice;
    }
#else
#endif
    return "C:\\";
}

void FileSystem::TransformNameIntoPathString(std::string& str)
{
    std::regex const filter("[^0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-""]");
    str = std::regex_replace(str, filter, "");
}

std::string FileSystem::GetFileName(const Path& path)
{
    std::string result = path.string();
    result = result.substr(result.find_last_of("/\\") + 1);
    return result;
}

bool FileSystem::Exists(const Path& path)
{
    return std::filesystem::exists(path);
}

bool FileSystem::IsFolder(const Path& path)
{
    return std::filesystem::is_directory(path);
}

bool FileSystem::HasExtension(const Path& path, const Path& extension)
{
    return path.extension() == extension;
}

bool FileSystem::HasAnyExtension(const Path& path, const std::vector<Path>& extensions)
{
    for(const auto& extension : extensions)
    {
        if(HasExtension(path, extension))
        {
            return true;
        }
    }

    return false;
}

bool FileSystem::HasAnyExtension(const Path& path, const char* const* extensions, size_t count)
{
    if(!extensions || count <= 0) return false;

    for(size_t i = 0; i < count; ++i)
    {
        if(HasExtension(path, extensions[i]))
        {
            return true;
        }
    }

    return false;
}

void FileSystem::ReplaceExtension(Path& path, const std::string& extension)
{
    path.replace_extension(extension);
}

void FileSystem::Delete(const Path& path)
{
    std::filesystem::remove(path);
}

bool FileSystem::OpenFile(const Path& path, File& outFile, FileMode mode)
{
    CORE_ASSERT(!path.empty(), "path is empty");

	int openMode = std::ios::in;

	switch (mode)
	{
        case FileMode::FILE_MODE_READ:
		openMode = std::ios::in;
		break;
	case FileMode::FILE_MODE_READ_BINARY:
		openMode = std::ios::in | std::ios::binary;
		break;
	case FileMode::FILE_MODE_WRITE:
		openMode = std::ios::out;
		break;
	case FileMode::FILE_MODE_WRITE_BINARY:
		openMode = std::ios::out | std::ios::binary;
		break;
	default:
		CORE_LOG_ERROR("failed to use the file mode, 'FILE_MODE_READ' mode will be used by default");
		break;
    }

    outFile.handle = std::fstream(path, openMode);
    outFile.path = path;

    if(!outFile.handle.is_open())
    {
        CORE_LOG_ERROR("failed to open file %s", path.c_str());
        return false;
    }

    return true;
}

void FileSystem::CloseFile(File& file)
{
    file.handle.close();
}

size_t FileSystem::GetFileSize(File& file)
{
    file.handle.seekg(0, std::ios::end);
    size_t bufferSize = file.handle.tellg();
    file.handle.seekg(0, std::ios::beg);
    return static_cast<size_t>(bufferSize);
}

}
