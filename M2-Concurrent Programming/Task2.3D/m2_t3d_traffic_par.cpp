/* This code is made by Anneshu Nag, Student ID-2210994760*/
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

struct TrafficData
{
    time_t timestamp;
    int light_id;
    int num_cars;
};

queue<TrafficData> data_queue;
mutex data_mutex;
condition_variable data_cv;
bool producer_finished = false;

void producer(ifstream &infile)
{
    string line;
    bool skipFirstLine = true; // Flag to skip the first line
    while (getline(infile, line))
    {
        if (skipFirstLine)
        {
            skipFirstLine = false;
            continue; // Skip the first line
        }

        stringstream ss(line);
        string timestamp_str, light_id_str, num_cars_str;
        getline(ss, timestamp_str, ',');
        getline(ss, light_id_str, ',');
        getline(ss, num_cars_str, ',');
        TrafficData td{
            .timestamp = stol(timestamp_str),
            .light_id = stoi(light_id_str),
            .num_cars = stoi(num_cars_str)};
        {
            lock_guard<mutex> lock(data_mutex);
            data_queue.push(td);
        }
        data_cv.notify_one();
    }

    // Notify the consumer that the producer has finished reading the input file
    {
        lock_guard<mutex> lock(data_mutex);
        producer_finished = true;
    }
    data_cv.notify_all();
}

void print_data(const unordered_map<int, int> &car_counts, const time_t &current_hour)
{
    // Report top N congested traffic lights
    vector<pair<int, int>> light_counts(car_counts.begin(), car_counts.end());
    sort(light_counts.begin(), light_counts.end(), [](const pair<int, int> &left, const pair<int, int> &right)
         { return left.second > right.second; });

    // Print the top N congested traffic lights
    for (int i = 0; i < min(5, static_cast<int>(light_counts.size())); i++)
    {
        cout << "     |Traffic Light " << light_counts[i].first << ": " << light_counts[i].second << " cars|" << endl;
    }
    cout << "     ---------------------------" << endl;
    cout << endl;
}

unordered_map<int, int> total_car_counts;
unordered_map<int, int> overall_counts; // To store overall congestion at each traffic light
int total_cars_per_hour = 0;            // To store the total cars passing through all traffic lights for each hour
int max_cars_for_the_hour = 0;
time_t max_congestion_hour;

void process_traffic_data(const TrafficData &td, unordered_map<int, int> &car_counts, time_t &current_hour)
{

    if (td.timestamp - current_hour >= 3600)
    {
        // Print the total cars passed for the current hour
        cout << "For the Hour: " << asctime(localtime(&current_hour));
        cout << " >> Total Cars Passed: " << total_cars_per_hour << " cars <<" << endl;
        if (total_cars_per_hour > max_cars_for_the_hour)
        {
            max_cars_for_the_hour = total_cars_per_hour;
            max_congestion_hour = current_hour;
        }
        print_data(car_counts, current_hour);
        // Update overall congestion counts for the last hour
        for (const auto &light_count : car_counts)
        {
            overall_counts[light_count.first] += light_count.second;
        }
        car_counts.clear();
        current_hour = td.timestamp - (td.timestamp % 3600);
        total_cars_per_hour = 0;
    }

    // Process data
    car_counts[td.light_id] += td.num_cars;
    total_car_counts[td.light_id] += td.num_cars;
    total_cars_per_hour += td.num_cars;
}

void consumer()
{
    unordered_map<int, int> car_counts;
    time_t current_hour = data_queue.front().timestamp / 3600 * 3600;

    while (true)
    {
        unique_lock<mutex> lock(data_mutex);
        data_cv.wait(lock, [&]
                     { return !data_queue.empty() || producer_finished; });

        while (!data_queue.empty())
        {
            TrafficData td = data_queue.front();
            data_queue.pop();
            lock.unlock(); // Release the lock during processing

            process_traffic_data(td, car_counts, current_hour);

            lock.lock(); // Reacquire the lock
        }

        if (producer_finished)
            break;
    }

    // Process the remaining data after producer finishes
    while (!data_queue.empty())
    {
        TrafficData td = data_queue.front();
        data_queue.pop();

        process_traffic_data(td, car_counts, current_hour);
    }

    // Report top N congested traffic lights for the last hour
    cout << "For the Hour: " << asctime(localtime(&current_hour));
    cout << " >> Total Cars Passed: " << total_cars_per_hour << " cars <<" << endl;
    if (total_cars_per_hour > max_cars_for_the_hour)
    {
        max_cars_for_the_hour = total_cars_per_hour;
        max_congestion_hour = current_hour;
    }
    print_data(car_counts, current_hour);

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
         << "Most Congested Hour: " << asctime(localtime(&max_congestion_hour)) << "Number of Cars Passed: " << max_cars_for_the_hour << " cars passed." << endl;
}

int main()
{
    const string data_file = "traffic_test_data.txt";
    const int top_n = 5;
    cout << "   -Congestion in Descending Order-" << endl;
    cout << "      -Congestion for each Hour-" << endl
         << endl;

    ifstream infile(data_file);
    if (!infile.is_open())
    {
        cerr << "Error opening file: " << data_file << endl;
        return 1;
    }

    // Create producer and consumer threads
    thread producer_thread(producer, ref(infile));
    thread consumer_thread(consumer);

    // Wait for threads to finish
    producer_thread.join();
    consumer_thread.join();

    return 0;
}
