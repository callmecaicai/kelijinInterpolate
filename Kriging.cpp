#include "Kriging.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace Eigen;
using namespace std;

Kriging::Kriging(double range, double sill, VariogramModel model)
    : range(range), sill(sill), model(model) {}

double Kriging::variogram(double distance) {
    switch (model) {
    case VariogramModel::Spherical:
        if (distance > range) {
            return sill;
        }
        else {
            double ratio = distance / range;
            return sill * (1.5 * ratio - 0.5 * pow(ratio, 3));
        }
    case VariogramModel::Gaussian:
        return sill * (1 - exp(-pow(distance / range, 2)));
    case VariogramModel::Exponential:
        return sill * (1 - exp(-distance / range));
    case VariogramModel::Linear:
        return sill * (distance / range);
    default:
        throw invalid_argument("Unsupported variogram model");
    }
}

double Kriging::calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double Kriging::interpolate(const std::vector<Point>& points, double x, double y) {
    int n = points.size();
    MatrixXd K(n + 1, n + 1);
    VectorXd k(n + 1);
    VectorXd weights(n + 1);

    // 构造克里金矩阵 K 和向量 k
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            K(i, j) = variogram(calculateDistance(points[i], points[j]));
        }
        K(i, n) = 1;
        K(n, i) = 1;
        k(i) = variogram(calculateDistance(points[i], Point{ 0, 0, x, y }));
    }
    K(n, n) = 0;
    k(n) = 1;

    // 求解权重
    weights = K.partialPivLu().solve(k);

    // 计算插值结果
    double result = 0;
    for (int i = 0; i < n; i++) {
        result += weights(i) * points[i].value;
    }

    return result;
}

vector<Point> Kriging::readPointsFromFile(const std::string& filename) {
    vector<Point> points;
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    string line;
    getline(file, line); // 跳过表头

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Point point;

        getline(ss, token, ',');
        point.objectid = stoi(token);

        getline(ss, token, ',');
        point.value = stod(token);

        getline(ss, token, ',');
        point.x = stod(token);

        getline(ss, token, ',');
        point.y = stod(token);

        points.push_back(point);
    }

    file.close();
    return points;
}

void Kriging::writePointsToFile(const std::string& filename, const std::vector<Point>& points) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    file << "OBJECTID,Value,X,Y\n";
    for (const auto& point : points) {
        file << point.objectid << "," << point.value << "," << point.x << "," << point.y << "\n";
    }

    file.close();
}
