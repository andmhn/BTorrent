#include "torrent_metadata.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

TEST_CASE("with single file torrent") {
  std::string filePath =
      TORRENT_FILES_PATH "linuxmint-22-xfce-64bit.iso.torrent";

  std::cout << "\nloading torrent with single file\n";
  bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(filePath);

  REQUIRE(torr.files().size() != 0);
  CHECK(torr.files().size() == 1);
  CHECK(!torr.comment().has_value());
  CHECK(torr.announceList().size() == 0);
  CHECK(torr.mainAnnounce().has_value());
}

TEST_CASE("with multiple file torrent") {
  std::string filePath = TORRENT_FILES_PATH "india-pocket-map_archive.torrent";

  std::cout << "\nloading torrent with multiple file\n";
  bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(filePath);

  CHECK(torr.files().size() > 1);
  CHECK(torr.comment().has_value());
  CHECK(torr.announceList().size() > 0);
  CHECK(torr.mainAnnounce().has_value());
}


std::string EncodeWithPercent(std::string str) {
  std::string percentEncodedString;
  size_t i = 0;
  for (auto c : str) {
    if ((i % 2) == 0) {
      percentEncodedString.push_back('%');
    }
    percentEncodedString.push_back(std::toupper(c));
    i++;
  }
  return percentEncodedString;
}
TEST_CASE("download torrent") {
  std::string filePath = TORRENT_FILES_PATH "india-pocket-map_archive.torrent ";

  std::cout << "\nDownloading torrent with single file\n";
  bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(filePath);

  // build http request url that can be used to get peers
  puts("\nyour tcp url :");
  std::cout << torr.mainAnnounce().value() 
            << "?info_hash=" << EncodeWithPercent(torr.infoHash())
            << "&left=" << torr.pieceLength()
            << "&peer_id=-PC0001-706887310628&uploaded=0&downloaded=0&port=6889&compact=1\n";

  // TODO : implement http handshake with tracker
}

int DaytimeClient(const char*);
TEST_CASE("TEST NETWORKING") {
  puts("\ntesting boost asio");
  puts("pinging to timeServerHost:  time-a-g.nist.gov");
  DaytimeClient("time-a-g.nist.gov");
}

// TODO: use it over the project

enum { LOG_INFO, LOG_ERROR, LOG_DEBUG, LOG_TRACE, LOG_WARNING };

void Log(int type, const char *fmt, ...);

#define LogInfo(...) Log(LOG_INFO, __VA_ARGS__)
#define LogError(...) Log(LOG_ERROR, __VA_ARGS__)
#define LogWarning(...) Log(LOG_WARNING, __VA_ARGS__)
#define LogDebug(...) Log(LOG_DEBUG, __VA_ARGS__)
#define LogTrace(...) Log(LOG_TRACE, __VA_ARGS__)

static const char *log_type_strings[] = {"INFO", "ERROR", "DEBUG", "TRACE",
                                         "WARNING"};
#include <cstdarg>

// Basic logging function
void Log(int type, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  printf("[%s] ", log_type_strings[type]);
  vprintf(fmt, args);
  printf("\n");

  va_end(args);
}