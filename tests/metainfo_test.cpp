#include "metainfo.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

static void print(std::string msg) { std::cout << msg << "\n"; }

TEST_CASE("check flie reading") {
  bt::TorrentMetadata t1 = bt::torrent_parser::ParseFromFile(
      "C:/Users/anand/Desktop/ShriHanumanChalisa_404_archive.torrent");
  for (auto t : t1.files()) {
    std::cout << t.GetRelativePathAsString()<< "\n";
  }

  print(t1.infoHash());

  bt::torrent_parser::ParseFromFile(
      "C:/Users/anand/Desktop/Shree Ram Stuti.torrent");

  print("");
  bt::TorrentMetadata t2 = bt::torrent_parser::ParseFromFile(
      "C:/Users/anand/Desktop/hash-library.torrent");

  print(t2.mainAnnounce().value());
  for (auto t : t2.announceList()) {
    print(t);
  }
}