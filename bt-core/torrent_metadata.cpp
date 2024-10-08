#include "torrent_metadata.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <format>

#include "external/bencode.hpp"
#include "external/sha1.h"

namespace bt {

TorrentFile::TorrentFile(std::vector<std::string> relativePath, long long size)
    : relativePath(relativePath), size(size) {
}

std::string TorrentFile::GetRelativePathAsString() {
    std::string pathNameBuider = "";
    char delem = '/';

    size_t foldersCount = relativePath.size() - 1;

    for (size_t i = 0; i < foldersCount; i++) {
        pathNameBuider.append(relativePath[i] + delem);
    }
    // append last element without delimeter
    pathNameBuider.append(relativePath.back());

    return pathNameBuider;
}

TorrentMetadata::TorrentMetadata(long long creationDate, long long pieceLength,
                                 long long piecesCount, std::string name, std::string infoHash,
                                 std::string piecesHashes, std::optional<std::string> comment,
                                 std::optional<std::string> createdBy,
                                 std::optional<std::string> mainAnnounce,
                                 std::vector<std::string> announceList,
                                 std::vector<TorrentFile> files)
    : _creationDate(creationDate),
      _pieceLength(pieceLength),
      _piecesCount(piecesCount),
      _name(name),
      _comment(comment),
      _createdBy(createdBy),
      _infoHash(infoHash),
      _piecesHashes(piecesHashes),
      _mainAnnounce(mainAnnounce),
      _announceList(announceList),
      _files(files) {
}

std::optional<long long> TorrentMetadata::creationDate() const {
    return _creationDate;
}

long long TorrentMetadata::pieceLength() const {
    return _pieceLength;
}

long long TorrentMetadata::piecesCount() const {
    return _piecesCount;
}

std::string TorrentMetadata::name() {
    return _name;
}

std::optional<std::string> TorrentMetadata::comment() {
    return _comment;
}

std::optional<std::string> TorrentMetadata::createdBy() {
    return _createdBy;
}

std::string TorrentMetadata::infoHash() {
    return _infoHash;
}

std::string TorrentMetadata::piecesHashes() {
    return _piecesHashes;
}

std::optional<std::string> TorrentMetadata::mainAnnounce() {
    return _mainAnnounce;
}

std::vector<std::string> TorrentMetadata::announceList() {
    return _announceList;
}

std::vector<TorrentFile> TorrentMetadata::files() {
    return _files;
}

/*
##################################################################
  bt::torrent_parser  implementation
###################################################################
*/
namespace torrent_parser {

template <typename T>
std::optional<T> _GetDictValue(bencode::data dict, std::string key);

static std::vector<std::string> _GetAnnounceList(bencode::data metaData);

static std::vector<TorrentFile> _ParseFiles(bencode::dict infoDict);

TorrentMetadata ParseFromFile(std::string path) {
    // load file to string and call Parse function

    std::ifstream torrentFile{path, std::ios::binary | std::ios::ate};
    if (!torrentFile.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    torrentFile.seekg(0);
    std::string metaInfo((std::istreambuf_iterator<char>(torrentFile)),
                         (std::istreambuf_iterator<char>()));
    return Parse(metaInfo);
}

TorrentMetadata Parse(std::string metaInfo) {
    // prepare 'variables' needed for constructing a TorrentMetadata object
    // also throw InvalidTorrentFile exception if necessary data are not present

    bencode::data metaData;
    try {
        metaData = bencode::decode(metaInfo);
    } catch (const bencode::decode_error &e) {
        std::string msg = "decode error: ";
        throw InvalidTorrentFile(msg + e.what());
    }

    bencode::dict infoDict;
    try {
        infoDict = std::get<bencode::dict>(std::get<bencode::dict>(metaData)["info"]);
    } catch (std::bad_variant_access) {
        throw InvalidTorrentFile("info key not present");
    }

    std::string piecesHashes;
    long long pieceLength;
    std::string name;
    try {
        piecesHashes = _GetDictValue<std::string>(infoDict, "pieces").value();
        pieceLength = _GetDictValue<bencode::integer>(infoDict, "piece length").value();
        name = _GetDictValue<std::string>(infoDict, "name").value();
    } catch (std::bad_optional_access) {
        throw InvalidTorrentFile("required keys not present");
    }

    long long creationDate =
        _GetDictValue<bencode::integer>(metaData, "creation date").value_or(-1);

    long long piecesCount = piecesHashes.length() / 20;

    std::string infoString = bencode::encode(std::get<bencode::dict>(metaData)["info"]);
    std::string infoHash = GetSha1Hash(infoString);

    std::optional<std::string> comment = _GetDictValue<std::string>(metaData, "comment");

    std::optional<std::string> createdBy = _GetDictValue<std::string>(metaData, "created by");

    std::optional<std::string> mainAnnounce = _GetDictValue<std::string>(metaData, "announce");

    std::vector<std::string> announceList = _GetAnnounceList(metaData);

    std::vector<TorrentFile> files = _ParseFiles(infoDict);

    return TorrentMetadata(creationDate, pieceLength, piecesCount, name, infoHash, piecesHashes,
                           comment, createdBy, mainAnnounce, announceList, files);
}

std::string GetSha1Hash(std::string text) {
    SHA1 sha1;
    return sha1(text);
}

/**
 * @brief It wraps std::get and catches std::bad_variant_access error
 * @brief Useful for skipping 'bad_variant_access' exception
 * @tparam T any bencode::basic_data type
 */
template <typename T>
std::optional<T> _GetDictValue(bencode::data dict, std::string key) {
    try {
        return std::get<T>(dict[key]);
    } catch (std::bad_variant_access) {
        LogInfo("torrent file does not contain attribute: {}", key);
    }
    return {};
}

/**
 * @param metaData is top level bencoded data of .torrent file
 * @return  announce-list from the top of torrent metaData dict
 */
static std::vector<std::string> _GetAnnounceList(bencode::data metaData) {
    std::optional<bencode::list> announceListOptional =
        _GetDictValue<bencode::list>(metaData, "announce-list");

    if (!announceListOptional.has_value()) {
        return {};
    }

    std::vector<std::string> announceList;
    for (bencode::data items : announceListOptional.value()) {
        for (bencode::data item : std::get<bencode::list>(items)) {
            announceList.emplace_back(std::get<bencode::string>(item));
        }
    }
    return announceList;
}

/**
 * @param infoDict is a dict from torrent metadata
 * @return list of multiple files from info-dict
 * @return list of single file if there is no 'files' key in info-dict
 * @throws InvalidTorrentFile
 */
static std::vector<TorrentFile> _ParseFiles(bencode::dict infoDict) {
    bencode::list filesList;

    try { // catches std::bad_optional_access

        // check for single file torrent
        try {
            filesList = std::get<bencode::list>(infoDict["files"]);
        } catch (std::bad_variant_access) {
            long long length = _GetDictValue<bencode::integer>(infoDict, "length").value();
            std::string fileName = _GetDictValue<std::string>(infoDict, "name").value();

            // return single file list from top of infoHash
            return {TorrentFile({fileName}, length)};
        }

        // build TorrentFile list
        std::vector<TorrentFile> torrentFilesBuilder;

        for (bencode::data file : filesList) {
            long long len = _GetDictValue<bencode::integer>(file, "length").value();
            std::vector<bencode::data> pathListData =
                _GetDictValue<bencode::list>(file, "path").value();

            std::vector<std::string> pathListBuilder;
            for (bencode::data p : pathListData) {
                pathListBuilder.emplace_back(std::get<bencode::string>(p));
            }
            torrentFilesBuilder.emplace_back(TorrentFile({pathListBuilder}, len));
        }

        return torrentFilesBuilder;

    } catch (std::bad_optional_access) {
        throw InvalidTorrentFile("required keys not present in files list");
    }
}

} // namespace torrent_parser
} // namespace bt