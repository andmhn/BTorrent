#pragma once

#include <optional>
#include <string>
#include <vector>

namespace bt {

class InvalidTorrentFile : public std::exception {
  public:
    InvalidTorrentFile(std::string desc) {
        _err += " (" + desc + ")";
    }
    InvalidTorrentFile() {
    }

    const char* what() const throw() {
        return _err.c_str();
    }

  private:
    std::string _err = "Invalid Torrent File";
};

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
    // clang-format off
  public:
    TorrentMetadata(long long creationDate,
                    long long pieceLength,
                    long long piecesCount,
                    std::string name,
                    std::string infoHash, 
                    std::string piecesHashes, 
                    std::optional<std::string> comment,
                    std::optional<std::string> createdBy,
                    std::optional<std::string> mainAnnounce,
                    std::vector<std::string> announceList,
                    std::vector<TorrentFile> files
    );
    // clang-format on

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
    long long _creationDate; // nullable null ? -1
    long long _pieceLength;  // required
    long long _piecesCount;
    std::string _name; // required
    std::string _infoHash;
    std::string _piecesHashes;                // required
    std::optional<std::string> _comment;      // nullable
    std::optional<std::string> _createdBy;    // nullable
    std::optional<std::string> _mainAnnounce; // nullable
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
 * @throws std::runtime_error if file could not be opened
 */
TorrentMetadata ParseFromFile(std::string path);

/**
 * @brief loads torrent metadata from bencoded metaInfo
 * @param metaInfo is bencoded string loaded from .torrent file
 * @return parsed torrent metadata
 * @throws bt::InvalidTorrentFile if metaInfo has missing required fields
 */
TorrentMetadata Parse(std::string metaInfo);

/**
 * @return SHA1 hash of text
 */
std::string GetSha1Hash(std::string text);
} // namespace torrent_parser

} // namespace bt