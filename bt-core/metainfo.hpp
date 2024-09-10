#pragma once

#include <list>
#include <optional>
#include <string>
#include <vector>

namespace bt {

/**
 * @brief represents a single file from files list
 */
class TorrentFile {
 public:
  /**
   * @brief list of path as nodes
   */
  std::vector<std::string> relativePath;

  /**
   * @brief size of file in bytes
   */
  long long size;

 public:
  TorrentFile(std::vector<std::string> relativePath, long long size);

  /**
   * @return concatinated string for relative path with delimeter '/'
   */
  std::string GetRelativePathAsString();
};

/**
 * @brief It represent all stored data in a .torrent file
 * @brief refer to metainfo spec http://www.bittorrent.org/beps/bep_0003.html
 */
class TorrentMetadata {
 public:
  TorrentMetadata(long long creationDate, long long pieceLength,
                  long long piecesCount, std::string name, std::string comment,
                  std::string createdBy, std::string infoHash,
                  std::string piecesHashes, std::string mainAnnounce,
                  std::vector<std::string> announceList,
                  std::vector<TorrentFile> files);

  /**
   * @return creation date of the torrent in unix format
   */
  std::optional<long long> creationDate() const;

  /**
   * @return  number of bytes in each piece
   */
  long long pieceLength() const;

  /**
   * @return count of pieces in torrent
   */
  long long piecesCount() const;

  /**
   * @return name of torrent
   */
  std::string name();

  /**
   * @return text comment of the author
   */
  std::optional<std::string> comment();

  /**
   * @return name and version of the program used to create .torrent
   */
  std::optional<std::string> createdBy();

  /**
   * @return the hash of the B-encoded meta-info dictionary of a torrent.
   */
  std::string infoHash();

  /**
   * @brief stores pieces hashes where every piece has 20 char length
   * @return concatenation of all 20 - byte SHA1 hash values, one per piece.
   */
  std::string piecesHashes();

  /**
   * @return main announce url for tracker or null for absense
   */
  std::optional<std::string> mainAnnounce();

  /**
   * @brief refer to http://bittorrent.org/beps/bep_0012.html
   * @return all tracker for announce
   */
  std::vector<std::string> announceList();

  /**
   * @return list of files, stored in this torrent
   */
  std::vector<TorrentFile> files();

 private:
  long long _creationDate;  // nullable
  long long _pieceLength;
  long long _piecesCount;
  std::string _name;
  std::string _comment;    // nullable
  std::string _createdBy;  // nullable
  std::string _infoHash;
  std::string _piecesHashes;
  std::string _mainAnnounce;
  std::vector<std::string> _announceList;
  std::vector<TorrentFile> _files;
};

/**
 * @brief function namespace for torrent parsers
 */
namespace torrent_parser {

/**
 * @brief loads torrent metadata from .torrent file
 * @param path for .torrent file
 * @return parsed torrent metadata
 */
TorrentMetadata ParseFromFile(std::string path);

/**
 * @brief loads torrent metadata from bencoded metaInfo
 * @param metaInfo is bencoded string loaded from .torrent file
 * @return parsed torrent metadata
 */
TorrentMetadata Parse(std::string metaInfo);

/**
 * @return SHA1 hash of text
 */
std::string GetSha1Hash(std::string text);
}  // namespace torrent_parser

}  // namespace bt