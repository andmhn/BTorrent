#include "torrent_metadata.hpp"
#include "utils.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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

TEST_CASE("testing parser with single file torrent") {
    std::string filePath = TORRENT_FILES_PATH "linuxmint-22-xfce-64bit.iso.torrent";

    LogTrace("loading torrent with single file: {}", filePath);
    bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(filePath);

    REQUIRE(torr.files().size() != 0);
    CHECK(torr.files().size() == 1);
    CHECK(!torr.comment().has_value());
    CHECK(torr.announceList().size() == 0);
    CHECK(torr.mainAnnounce().has_value());
}

TEST_CASE("testing parser with multiple file torrent") {
    std::string filePath = TORRENT_FILES_PATH "india-pocket-map_archive.torrent";

    LogTrace("loading torrent with multiple file: {}", filePath);
    bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(filePath);

    CHECK(torr.files().size() > 1);
    CHECK(torr.comment().has_value());
    CHECK(torr.announceList().size() > 0);
    CHECK(torr.mainAnnounce().has_value());

    SUBCASE("download torrent") {
        // build http request url that can be used to get peers
        std::string httpString = torr.mainAnnounce().value() +
                                 "?info_hash=" + EncodeWithPercent(torr.infoHash()) +
                                 "&left=" + std::to_string(torr.pieceLength()) +
                                 "&peer_id=-PC0001-706887310628&uploaded=0&downloaded=0&port="
                                 "6889&compact=1";

        LogTrace("your http url :\n\t {}", httpString);

        // TODO : implement http handshake with tracker
    }
}

TEST_CASE("Testing Parser with various Invalid files") {
    puts("");
    std::string filePath[] = {TORRENT_FILES_PATH "non_existant_file.torrent",
                              TORRENT_FILES_PATH "plain_text.txt", TORRENT_FILES_PATH "binary.png"};
    for (std::string path : filePath) {
        try {
            bt::TorrentMetadata torr = bt::torrent_parser::ParseFromFile(path);
        } catch (std::exception &e) {
            LogError("{} : {}", path, e.what());
        }
        CHECK_THROWS(bt::torrent_parser::ParseFromFile(path));
        puts("");
    }
}

int DaytimeClient(const char *);

TEST_CASE("TEST NETWORKING") {
    puts("");
    LogInfo("testing boost asio");
    LogInfo("pinging to timeServerHost:  time-a-g.nist.gov");
    DaytimeClient("time-a-g.nist.gov");
}
