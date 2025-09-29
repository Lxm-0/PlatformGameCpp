#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace std;



class POoints {
public:
    int score;

    POoints() : score(0) {}

    void add(int value) {
        score += value;
    }

    void reset() {
        score = 0;
    }

    void save(const string& filename = "points.txt") {
        ofstream file(filename, ios::trunc);
        if (file.is_open()) {
            file << score;
        }
    }

    void load(const string& filename = "points.txt") {
        ifstream file(filename);
        if (file.is_open()) {
            file >> score;
        }
    }
};