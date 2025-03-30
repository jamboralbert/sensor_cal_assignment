#ifndef DBWPROCESSOR_H
#define DBWPROCESSOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

struct DBWData {
    double timestamp;
    float speed, yaw_rate, v_front_left, v_front_right, v_rear_left, v_rear_right;
};

struct NormalizedDBWData {
    float timestamp;
    float speed;
};

class DBWProcessor {
private:
    std::vector<DBWData> dbwData;

public:
    bool parseDBWFile(const std::string& filename);
    bool cleanDBWLine(const std::string& line, DBWData& dbw);
    const std::vector<DBWData>& getData() const;
    std::vector<NormalizedDBWData> normalizeDBWData() const;
};

#endif // DBWPROCESSOR_H
