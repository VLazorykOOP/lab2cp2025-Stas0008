#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <cmath>
#include <mutex>

using namespace std;

const double w = 10.0;
const double h = 10.0;
const double V = 1.0; 
const int numEntities = 5;

// М'ютекс для синхронізації виводу
mutex mtx;

pair<double, double> generateRandomPoint(double xMin, double xMax, double yMin, double yMax);

class Entity {
public:
    double x, y;
    double targetX, targetY;
    bool moving;

    Entity(double startX, double startY, double targetX, double targetY)
        : x(startX), y(startY), targetX(targetX), targetY(targetY), moving(true) {}

    void move() {
        double dx = targetX - x;
        double dy = targetY - y;
        double distance = sqrt(dx * dx + dy * dy);

        if (distance < V) {
            x = targetX;
            y = targetY;
            moving = false;
        }
        else {
            double directionX = dx / distance;
            double directionY = dy / distance;
            x += V * directionX;
            y += V * directionY;
        }
    }

    void print(const string& type) {
        lock_guard<mutex> lock(mtx);
        cout << type << " at (" << x << ", " << y << ") moving: " << (moving ? "yes" : "no") << endl;
    }
};

pair<double, double> generateRandomPoint(double xMin, double xMax, double yMin, double yMax) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> disX(xMin, xMax);
    uniform_real_distribution<> disY(yMin, yMax);
    return { disX(gen), disY(gen) };
}

void simulateLegalEntities(vector<Entity>& entities) {
    for (auto& entity : entities) {
        while (entity.moving) {
            entity.move();
            entity.print("Legal");
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

void simulatePhysicalEntities(vector<Entity>& entities) {
    for (auto& entity : entities) {
        while (entity.moving) {
            entity.move();
            entity.print("Physical");
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

int main() {

    vector<Entity> legalEntities;
    vector<Entity> physicalEntities;

    for (int i = 0; i < numEntities; ++i) {
        double startX, startY, targetX, targetY;
        tie(startX, startY) = generateRandomPoint(0, w / 2, 0, h / 2); 
        tie(targetX, targetY) = generateRandomPoint(0, w / 2, 0, h / 2);
        legalEntities.emplace_back(startX, startY, targetX, targetY);

        tie(startX, startY) = generateRandomPoint(w / 2, w, h / 2, h);
        tie(targetX, targetY) = generateRandomPoint(w / 2, w, h / 2, h);
        physicalEntities.emplace_back(startX, startY, targetX, targetY);

        if (startX == targetX && startY == targetY) {
            legalEntities.back().moving = false;
        }
        if (startX == targetX && startY == targetY) {
            physicalEntities.back().moving = false;
        }
    }

    thread legalThread(simulateLegalEntities, ref(legalEntities));
    thread physicalThread(simulatePhysicalEntities, ref(physicalEntities));

    legalThread.join();
    physicalThread.join();

    cout << "Simulation completed!" << endl;

    return 0;
}