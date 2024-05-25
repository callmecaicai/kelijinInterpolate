#ifndef KRIGING_H
#define KRIGING_H

#include <vector>
#include <string>
#include <Eigen/Dense>

struct Point {
    int objectid;
    double value;
    double x, y;
};

enum class VariogramModel {
    Spherical,
    Gaussian,
    Exponential,
    Linear
};

class Kriging {
public:
    Kriging(double range, double sill, VariogramModel model);
    double interpolate(const std::vector<Point>& points, double x, double y);
    std::vector<Point> readPointsFromFile(const std::string& filename);
    void writePointsToFile(const std::string& filename, const std::vector<Point>& points);

private:
    double variogram(double distance);
    double calculateDistance(const Point& p1, const Point& p2);

    double range;
    double sill;
    VariogramModel model;
};

#endif // KRIGING_H
