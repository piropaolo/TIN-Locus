#include "gtest/gtest.h"
#include "../ClientInfo.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <iostream>

#include "buffer/Converter.h"

#define PRINT2

using namespace std;
using namespace cereal;
using namespace buffer;

void fill(ClientInfo &clientInfo) {
    clientInfo.setName("name");

    clientInfo.addPosition(0,0,0);
    clientInfo.addPosition(0,0,1);
    clientInfo.addPosition(0,0,2);
    clientInfo.addPosition(0,0,3);

    clientInfo.addObserver(1);
    clientInfo.addObserver(2);

    clientInfo.addWatcher(1);
    clientInfo.addWatcher(2);
}

TEST(ClientInfo_Constructor, Initialize) {
    EXPECT_NO_THROW(ClientInfo());
    EXPECT_NO_THROW(ClientInfo(std::vector<unsigned char>(4, 0)));
    EXPECT_NO_THROW(ClientInfo(toByteVector("0000")));
}

TEST(ClientInfo_Serialize, JSON) {
    ClientInfo clientInfo(toByteVector("0000"));
    fill(clientInfo);

    stringstream ss;
    JSONOutputArchive oarchive(ss);

    EXPECT_NO_THROW(oarchive(clientInfo));
#ifdef PRINT
    cout << ss.str() << endl;
#endif
}

TEST(ClientInfo_Deserialize, JSON) {
    stringstream ss;
    ClientInfo oclientInfo(toByteVector("0000"));
    fill(oclientInfo);

    ClientInfo iclientInfo;
    {
        JSONOutputArchive oarchive(ss);
        oarchive(oclientInfo);
    }
    {
        JSONInputArchive iarchive(ss);
        iarchive(iclientInfo);
    }
    EXPECT_EQ(oclientInfo, iclientInfo);
}

TEST(ClientInfo_Serialize, Binary) {
    ClientInfo clientInfo(toByteVector("0000"));
    fill(clientInfo);

    stringstream ss;
    BinaryOutputArchive archive(ss);

    EXPECT_NO_THROW(archive(clientInfo));
#ifdef PRINT
    cout << ss.str() << endl;
#endif
}

TEST(ClientInfo_Deserialize, Binary) {
    stringstream ss;
    ClientInfo oclientInfo(toByteVector("0000"));
    fill(oclientInfo);

    ClientInfo iclientInfo;
    {
        BinaryOutputArchive oarchive(ss);
        oarchive(oclientInfo);
    }
    {
        BinaryInputArchive iarchive(ss);
        iarchive(iclientInfo);
    }
    EXPECT_EQ(oclientInfo, iclientInfo);
}

TEST(ClientInfo_Serialize, PortableBinary) {
    ClientInfo clientInfo(toByteVector("0000"));
    fill(clientInfo);

    stringstream ss;
    PortableBinaryOutputArchive archive(ss);

    EXPECT_NO_THROW(archive(clientInfo));
#ifdef PRINT
    cout << ss.str() << endl;
#endif
}

TEST(ClientInfo_Deserialize, PortableBinary) {
    stringstream ss;
    ClientInfo oclientInfo(toByteVector("0000"));
    fill(oclientInfo);

    ClientInfo iclientInfo;
    {
        PortableBinaryOutputArchive oarchive(ss);
        oarchive(oclientInfo);
    }
    {
        PortableBinaryInputArchive iarchive(ss);
        iarchive(iclientInfo);
    }
    EXPECT_EQ(oclientInfo, iclientInfo);
}

