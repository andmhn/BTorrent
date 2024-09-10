#include "metainfo.hpp"

#include <fstream>
#include <sstream>

#include "external/bencode.hpp"
#include "external/sha1.h"

namespace bt {

TorrentFile::TorrentFile(std::vector<std::string> relativePath, long long size)
    : relativePath(relativePath), size(size) {}

std::string TorrentFile::GetRelativePathAsString() {
  std::string pathNameBuider = "";
  char delem = '/';

  for (size_t i = 0; i < relativePath.size() - 1; i++) {
    pathNameBuider.append(relativePath[i] + delem);
  }
  pathNameBuider.append(relativePath.back());

  return pathNameBuider;
}

TorrentMetadata::TorrentMetadata(long long creationDate, long long pieceLength,
                                 long long piecesCount, std::string name,
                                 std::string comment, std::string createdBy,
                                 std::string infoHash, std::string piecesHashes,
                                 std::string mainAnnounce,
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
      _files(files) {}

std::optional<long long> TorrentMetadata::creationDate() const {
  if (_creationDate != 0) {
    return _creationDate;
  }
  return {};
}

long long TorrentMetadata::pieceLength() const { return _pieceLength; }

long long TorrentMetadata::piecesCount() const { return _piecesCount; }

std::string TorrentMetadata::name() { return _name; }

std::optional<std::string> TorrentMetadata::comment() {
  if (_comment.length() > 0) {
    return _comment;
  }
  return {};
}

std::optional<std::string> TorrentMetadata::createdBy() {
  if (_createdBy.length() > 0) {
    return _createdBy;
  }
  return {};
}

std::string TorrentMetadata::infoHash() { return _infoHash; }

std::string TorrentMetadata::piecesHashes() { return _piecesHashes; }

std::optional<std::string> TorrentMetadata::mainAnnounce() {
  if (_mainAnnounce.length() > 0) {
    return _mainAnnounce;
  }
  return {};
}

std::vector<std::string> TorrentMetadata::announceList() {
  return _announceList;
}

std::vector<TorrentFile> TorrentMetadata::files() { return _files; }

/*
##################################################################
  bt::torrent_parser  implementation
###################################################################
*/
namespace torrent_parser {

template <typename T>
T _GetDictValue(bencode::data dict, std::string key);

static std::vector<std::string> _GetAnnounceList(bencode::data metaData);

static std::vector<TorrentFile> _ParseFiles(bencode::dict infoDict);

TorrentMetadata ParseFromFile(std::string path) {
  std::ifstream torrentFile{path, std::ios::binary | std::ios::ate};
  if (!torrentFile.is_open()) {
    throw std::runtime_error("Could not open file\n");
  }

  torrentFile.seekg(0);
  std::string metaInfo((std::istreambuf_iterator<char>(torrentFile)),
                       (std::istreambuf_iterator<char>()));
  return Parse(metaInfo);
}

TorrentMetadata Parse(std::string metaInfo) {
  bencode::data metaData = bencode::decode(metaInfo);
  bencode::dict infoDict =
      std::get<bencode::dict>(std::get<bencode::dict>(metaData)["info"]);
  std::string infoString =
      bencode::encode(std::get<bencode::dict>(metaData)["info"]);
  std::string infoHash = GetSha1Hash(infoString);

  long long creationDate =
      _GetDictValue<bencode::integer>(metaData, "creation date");
  std::string comment = _GetDictValue<std::string>(metaData, "comment");
  std::string createdBy = _GetDictValue<std::string>(metaData, "created by");
  std::string mainAnnounce = _GetDictValue<std::string>(metaData, "announce");
  std::vector<std::string> announceList = _GetAnnounceList(metaData);

  std::string name = _GetDictValue<std::string>(infoDict, "name");
  std::string piecesHashes = _GetDictValue<std::string>(infoDict, "pieces");
  std::vector<TorrentFile> files = _ParseFiles(infoDict);

  long long pieceLength =
      _GetDictValue<bencode::integer>(infoDict, "piece length");
  long long piecesCount = piecesHashes.length() / 20;

  return TorrentMetadata(creationDate, pieceLength, piecesCount, name, comment,
                         createdBy, infoHash, piecesHashes, mainAnnounce,
                         announceList, files);
}

std::string GetSha1Hash(std::string text) {
  SHA1 sha1;
  return sha1(text);
}

// TODO: make it return std::optional
/**
 * @brief It wraps std::get and catches std::bad_variant_access error
 * @brief Useful for skipping 'bad_variant_access' exception
 * @tparam T any bencode::basic_data type
 */
template <typename T>
T _GetDictValue(bencode::data dict, std::string key) {
  try {
    return std::get<T>(dict[key]);
  } catch (std::bad_variant_access &e) {
    std::cerr << "[Info]: " << e.what() << " for key: " + key << "\n";
  }
  return T();
}

/**
 * @param metaData is top level bencoded data of .torrent file
 * @return  announce-list from the top of torrent metaData dict
 */
static std::vector<std::string> _GetAnnounceList(bencode::data metaData) {
  std::vector<std::string> announceList;
  std::vector list = _GetDictValue<bencode::list>(metaData, "announce-list");

  for (bencode::data items : list) {
    for (bencode::data item : std::get<bencode::list>(items)) {
      announceList.emplace_back(std::get<bencode::string>(item));
    }
  }
  return announceList;
}

/**
 * @param infoDict is a dict from torrent metadata
 * @return list of files from info-dict
 */
static std::vector<TorrentFile> _ParseFiles(bencode::dict infoDict) {
  bencode::list filesList;
  std::vector<TorrentFile> torrentFilesBuilder;
  bool containsManyFiles = true;

  try {
    filesList = std::get<bencode::list>(infoDict["files"]);
  } catch (std::bad_variant_access) {
    std::cerr << "[Info]: torrent contains a single file\n";
    containsManyFiles = false;
  }

  if (!containsManyFiles) {
    long long length = _GetDictValue<bencode::integer>(infoDict, "length");
    std::string fileName = _GetDictValue<std::string>(infoDict, "name");

    torrentFilesBuilder.emplace_back(TorrentFile({fileName}, length));
  }

  for (bencode::data file : filesList) {
    long long len = _GetDictValue<bencode::integer>(file, "length");
    std::vector<bencode::data> pathListData =
        _GetDictValue<bencode::list>(file, "path");

    std::vector<std::string> pathListBuilder;
    for (bencode::data p : pathListData) {
      pathListBuilder.emplace_back(std::get<bencode::string>(p));
    }
    torrentFilesBuilder.emplace_back(TorrentFile({pathListBuilder}, len));
  }
  return torrentFilesBuilder;
}

}  // namespace torrent_parser
}  // namespace bt