#include "IMUProcessor.h"

using namespace std;

bool IMUProcessor::parseIMUFile(const string& filename) {
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
        IMUData imu;
        if (cleanIMULine(line, imu)) {
            imuData.push_back(imu);
        }
    }

    file.close();
    return !imuData.empty();
}

bool IMUProcessor::cleanIMULine(const string& line, IMUData& imu) {
    stringstream ss(line);
    string temp;
    const float accuracyThreshold = 0.5; // Példa küszöbérték

    // Parse timestamp
    getline(ss, temp, ',');
    try {
        imu.timestamp = stod(temp);
    }
    catch (const exception&) {
        return false; // Discard row if timestamp is invalid
    }

    // Parse remaining float values
    float* fields[] = { &imu.gyro_x, &imu.gyro_y, &imu.gyro_z,
                        &imu.acc_x, &imu.acc_y, &imu.acc_z,
                        &imu.conf_gyro, &imu.conf_acc };

    for (int i = 0; i < 8; i++) {
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

    // Check accuracy threshold
    if (imu.conf_acc < accuracyThreshold || imu.conf_gyro < accuracyThreshold) {
        return false; // Discard row if accuracy is too low
    }

    return true;
}

std::vector<NormalizedIMUData> IMUProcessor::normalizeIMUData() const {
    std::vector<NormalizedIMUData> normalizedData;
    if (imuData.empty()) return normalizedData;

    double startTime = imuData.front().timestamp;

    for (const auto& imu : imuData) {
        double normTimestamp = imu.timestamp - startTime;

        // Kerekítés 4 tizedesjegyre
        normTimestamp = std::round(normTimestamp * 10000.0) / 10000.0;
        float accX = std::round(imu.acc_x * 10000.0) / 10000.0;
        float accY = std::round(imu.acc_y * 10000.0) / 10000.0;
        float accZ = std::round(imu.acc_z * 10000.0) / 10000.0;

        normalizedData.push_back({ normTimestamp, accX, accY, accZ });
    }

    return normalizedData;
}
const std::vector<IMUData>& IMUProcessor::getData() const {
    return imuData;
}
