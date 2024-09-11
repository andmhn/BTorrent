#include "metainfo.hpp"

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
