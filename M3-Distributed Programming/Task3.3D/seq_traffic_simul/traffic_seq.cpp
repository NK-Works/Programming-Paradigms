/* This code is made by Anneshu Nag, Student ID-2210994760*/
// Importing the necessary libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct TrafficData
{
    time_t timestamp;
    int light_id;
    int num_cars;
};

bool operator<(const TrafficData &left, const TrafficData &right)
{
    return left.timestamp < right.timestamp;
}

int main()
{
    const string data_file = "traffic_test_data.txt";
    const int top_n = 5;

    cout << "   -Congestion in Descending Order-" << endl;
    cout << "      -Congestion for each Hour-" << endl
         << endl;

    ifstream infile(data_file);

    // Skip the first line
    string header;
    getline(infile, header);

    // Read data into vector
    vector<TrafficData> data;
    string line;
    while (getline(infile, line))
    {
        stringstream ss(line);
        string timestamp_str, light_id_str, num_cars_str;
        getline(ss, timestamp_str, ',');
        getline(ss, light_id_str, ',');
        getline(ss, num_cars_str, ',');
        TrafficData td{
            .timestamp = stol(timestamp_str),
            .light_id = stoi(light_id_str),
            .num_cars = stoi(num_cars_str)};
        data.push_back(td);
    }
    auto start = high_resolution_clock::now(); // Start time measurement

    // Sort data by timestamp
    sort(data.begin(), data.end());
    time_t current_hour = data.front().timestamp / 3600 * 3600;
    int total_cars_per_hour = 0; // To store the total cars passing through all traffic lights for each hour
    int max_cars_for_the_hour = 0;
    time_t max_congestion_hour = data.front().timestamp / 3600 * 3600;
    // Process data hourly
    unordered_map<int, int> car_counts;
    unordered_map<int, int> overall_counts; // To store overall congestion at each traffic light
    for (const TrafficData &td : data)
    {
        if (td.timestamp - current_hour >= 3600)
        {
            // Report top N congested traffic lights
            vector<pair<int, int>> light_counts(car_counts.begin(), car_counts.end());
            sort(light_counts.begin(), light_counts.end(), [](const pair<int, int> &left, const pair<int, int> &right)
                 { return left.second > right.second; });

            // Print the total cars passed for the current hour
            cout << "For the Hour: " << asctime(localtime(&current_hour));
            cout << " >> Total Cars Passed: " << total_cars_per_hour << " cars <<" << endl;
            if (total_cars_per_hour > max_cars_for_the_hour)
            {
                max_cars_for_the_hour = total_cars_per_hour;
                max_congestion_hour = current_hour;
            }

            // Print the top N congested traffic lights
            for (int i = 0; i < min(top_n, static_cast<int>(light_counts.size())); i++)
            {
                cout << "     |Traffic Light " << light_counts[i].first << ": " << light_counts[i].second << " cars|" << endl;
            }
            cout << "     ---------------------------" << endl;
            cout << endl;

            // Update overall congestion counts for each traffic light
            for (const auto &light_count : car_counts)
            {
                overall_counts[light_count.first] += light_count.second;
            }

            // Reset car counts and total cars for the next hour
            car_counts.clear();
            total_cars_per_hour = 0;
            current_hour = td.timestamp - (td.timestamp % 3600);
        }

        car_counts[td.light_id] += td.num_cars;
        total_cars_per_hour += td.num_cars;
    }

    // Report top N congested traffic lights for the last hour
    vector<pair<int, int>> light_counts(car_counts.begin(), car_counts.end());
    sort(light_counts.begin(), light_counts.end(), [](const pair<int, int> &left, const pair<int, int> &right)
         { return left.second > right.second; });

    // Print the total cars passed for the last hour
    cout << "For the Hour: " << asctime(localtime(&current_hour));
    cout << "  >> Total Cars Passed: " << total_cars_per_hour << " cars <<" << endl;
    if (total_cars_per_hour > max_cars_for_the_hour)
    {
        max_cars_for_the_hour = total_cars_per_hour;
        max_congestion_hour = current_hour;
    }

    // Print the top N congested traffic lights for the last hour
    for (int i = 0; i < min(top_n, static_cast<int>(light_counts.size())); i++)
    {
        cout << "     |Traffic Light " << light_counts[i].first << ": " << light_counts[i].second << " cars|" << endl;
    }
    cout << "     ---------------------------" << endl;

    // Update overall congestion counts for the last hour
    for (const auto &light_count : car_counts)
    {
        overall_counts[light_count.first] += light_count.second;
    }

    // Update overall congestion counts for the last hour
    vector<pair<int, int>> overall_counts_vector(overall_counts.begin(), overall_counts.end());
    sort(overall_counts_vector.begin(), overall_counts_vector.end(), [](const pair<int, int> &left, const pair<int, int> &right)
         { return left.second > right.second; });

    // Print the overall congestion at each traffic light in descending order
    cout << "-Overall Congestion at Each Traffic Light-" << endl;
    for (const auto &overall_count : overall_counts_vector)
    {
        cout << "   |Traffic Light " << overall_count.first << ": " << overall_count.second << " total cars|" << endl;
    }
    cout << "   -----------------------------------" << endl;

    // Print the most congested hour
    cout << endl
         << "Most Congested Hour: " << asctime(localtime(&max_congestion_hour)) << "Number of Cars Passed: " << max_cars_for_the_hour << " cars passed.\n" << endl;

    auto stop = high_resolution_clock::now(); // Stop time measurement
    auto duration = duration_cast<microseconds>(stop - start); // Calculate duration
    std::cout << ">> Time taken by function: " << duration.count() << " microseconds <<" << std::endl;

    infile.close();
    return 0;
}
