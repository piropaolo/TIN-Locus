#include "Database.h"

#include <cereal/archives/json.hpp>
//#include <cereal/archives/binary.hpp>
//#include <cereal/archives/portable_binary.hpp>
#include <fstream>

using namespace cereal;

Database::Database() {
    //load
    std::ifstream is("ClientDataManager.json");
    JSONInputArchive archive(is);
    archive(clientDataManager);
}

Database::~Database() {
    //save
    std::ofstream os("ClientDataManager.json");
    JSONOutputArchive archive(os);
    archive(clientDataManager);
}

Database &Database::getInstance() {
    static Database database;
    return database;
}

ClientDataManager &Database::getClientDataManager() {
    return clientDataManager;
}
