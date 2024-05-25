#include <iostream>
#include <string>
#include "Kriging.h"

using namespace std;

VariogramModel getModelFromUserInput() {
    cout << "Select Variogram Model:" << endl;
    cout << "1. Spherical" << endl;
    cout << "2. Gaussian" << endl;
    cout << "3. Exponential" << endl;
    cout << "4. Linear" << endl;
    cout << "Enter your choice (1-4): ";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        return VariogramModel::Spherical;
    case 2:
        return VariogramModel::Gaussian;
    case 3:
        return VariogramModel::Exponential;
    case 4:
        return VariogramModel::Linear;
    default:
        throw invalid_argument("Invalid choice. Please select a number between 1 and 4.");
    }
}

int main() {
    try {
        // 读取输入文件中的数据点
        string inputFile = "C:\\Users\\Sun\\source\\repos\\ImageProcessing\\Point_IN.txt";
        string outputFile = "C:\\Users\\Sun\\source\\repos\\ImageProcessing\\Point_OUT.txt";

        // 半变异函数参数
        double range = 1000;
        double sill = 1000;

        // 获取用户选择的半变异函数模型
        VariogramModel model = getModelFromUserInput();

        Kriging kriging(range, sill, model);
        vector<Point> points = kriging.readPointsFromFile(inputFile);

        // 插值范围
        double minX = -1500, maxX = 1500, minY = -1500, maxY = 1500;
        double step = 100;

        // 插值后的点集合
        vector<Point> interpolatedPoints;
        int objectid = 1;

        // 对所有点进行插值
        for (double x = minX; x <= maxX; x += step) {
            for (double y = minY; y <= maxY; y += step) {
                double interpolatedValue = kriging.interpolate(points, x, y);
                interpolatedPoints.push_back(Point{ objectid++, interpolatedValue, x, y });
            }
        }

        // 将插值结果写入输出文件
        kriging.writePointsToFile(outputFile, interpolatedPoints);

        cout << "Interpolation completed and results written to " << outputFile << endl;
    }
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
