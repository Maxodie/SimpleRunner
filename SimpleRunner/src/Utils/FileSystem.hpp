#pragma once
#include "Core/Core.hpp"

namespace SR
{

using Path = std::filesystem::path;
using DirectoryIterator = std::filesystem::directory_iterator;

enum class FileMode
{
	FILE_MODE_READ,
	FILE_MODE_READ_BINARY,
	FILE_MODE_WRITE,
	FILE_MODE_WRITE_BINARY,
};


struct File
{
    Path path;
    std::fstream handle;
};

class FileSystem
{
public:
    static bool SyncReadAtPathAsString(const Path& path, std::string& outData);
    static bool SyncWriteAtPathAsString(const Path& path, const std::string& data);

    static bool CreateFolder(const Path& path, const std::string& name);
    static bool CreateFolder(const Path& path);

    static bool SyncReadFileAsByte(File& file, char* outBuffer, size_t bufferSize, size_t& outrBytesRead);
    static bool SyncWriteFileAsByte(File& file, const char* buffer);

    static Path GetPersistentDataPath();

    static void TransformNameIntoPathString(std::string& str);
    static std::string GetFileName(const Path& path);
    static bool Exists(const Path& path);
    static bool IsFolder(const Path& path);
    static bool HasExtension(const Path& path, const Path& extension);
    static bool HasAnyExtension(const Path& path, const std::vector<Path>& extensions);
    static bool HasAnyExtension(const Path& path, const char* const* extensions, size_t count);
    static void ReplaceExtension(Path& path, const std::string& extension);

    static void Delete(const Path& path);
private:
    static bool OpenFile(const Path& path, File& outFile, FileMode mode);
    static void CloseFile(File& file);
    [[nodiscard]] static size_t GetFileSize(File& file);
};

}
