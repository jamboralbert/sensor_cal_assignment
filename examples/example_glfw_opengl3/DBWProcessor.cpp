#include "DBWProcessor.h"

using namespace std;

bool DBWProcessor::parseDBWFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Error] Cannot open file: " << filename << endl;
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cerr << "[Error] File is empty or unreadable!" << endl;
        return false;
    }
    getline(file, line); // Skip header line

    while (getline(file, line)) {
        DBWData dbw;
        if (cleanDBWLine(line, dbw)) {
            dbwData.push_back(dbw);
        }
    }

    file.close();
    return !dbwData.empty();
}

bool DBWProcessor::cleanDBWLine(const string& line, DBWData& dbw) {
    stringstream ss(line);
    string temp;

    // Parse timestamp
    getline(ss, temp, ',');
    try {
        dbw.timestamp = stod(temp);
    }
    catch (const exception&) {
        return false; // Discard row if timestamp is invalid
    }

    // Parse remaining float values
    float* fields[] = { &dbw.speed, &dbw.yaw_rate, &dbw.v_front_left,
                        &dbw.v_front_right, &dbw.v_rear_left, &dbw.v_rear_right };

    for (int i = 0; i < 6; i++) {
        if (!getline(ss, temp, ',') || temp.empty() || temp == "-" || temp == "+") {
            return false; // Discard row if field is missing or invalid
        }
        try {
            *fields[i] = stof(temp);
        }
        catch (const exception&) {
            return false; // Discard row if any field is invalid
        }
    }

    return true;
}

vector<NormalizedDBWData> DBWProcessor::normalizeDBWData() const {
    vector<NormalizedDBWData> normalizedData;
    if (dbwData.empty()) return normalizedData;

    double startTime = dbwData.front().timestamp;

    for (const auto& dbw : dbwData) {
        double normTimestamp = dbw.timestamp - startTime;

        // Kerekítés 4 tizedesjegyre
        normTimestamp = round(normTimestamp * 10000.0) / 10000.0;
        float normSpeed = round(dbw.speed * 10000.0) / 10000.0;

        normalizedData.push_back({ normTimestamp, normSpeed });
    }

    return normalizedData;
}

const vector<DBWData>& DBWProcessor::getData() const {
    return dbwData;
}
