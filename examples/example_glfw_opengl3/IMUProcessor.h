#ifndef IMUPROCESSOR_H
#define IMUPROCESSOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <cmath>

struct IMUData {
    double timestamp;
    float gyro_x, gyro_y, gyro_z;
    float acc_x, acc_y, acc_z;
    float conf_gyro, conf_acc;
};

struct NormalizedIMUData {
    float timestamp;
    float acc_x, acc_y, acc_z;
};

class IMUProcessor {
private:
    std::vector<IMUData> imuData;

public:
    bool parseIMUFile(const std::string& filename);
    bool cleanIMULine(const std::string& line, IMUData& imu);
    const std::vector<IMUData>& getData() const;
    std::vector<NormalizedIMUData> normalizeIMUData() const;
};

#endif // IMUPROCESSOR_H
