/* This code is made by Anneshu Nag, Student ID-2210994760*/
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct TrafficData
{
    time_t timestamp;
    int light_id;
    int num_cars;
};

void producer(ifstream &infile, int rank, int num_processes)
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

        // Send the TrafficData to the consumer process
        int dest_process = 1 + (td.light_id % (num_processes - 1)); // Distribute data based on traffic light id
        MPI_Send(&td, sizeof(TrafficData), MPI_BYTE, dest_process, 0, MPI_COMM_WORLD);
    }

    // Send termination signal to the consumer processes
    TrafficData end_signal{.timestamp = -1, .light_id = -1, .num_cars = -1};
    for (int i = 1; i < num_processes; ++i)
    {
        MPI_Send(&end_signal, sizeof(TrafficData), MPI_BYTE, i, 0, MPI_COMM_WORLD);
    }
}

unordered_map<int, int> total_car_counts;
unordered_map<int, int> overall_counts; // To store overall congestion at each traffic light
int total_cars_per_hour = 0;            // To store the total cars passing through all traffic lights for each hour
int max_cars_for_the_hour = 0;
time_t max_congestion_hour;


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


void process_traffic_data(const TrafficData &td, unordered_map<int, int> &car_counts, time_t &current_hour)
{

    if (current_hour == 0)
    {
        current_hour = td.timestamp -(td.timestamp % 3600);
    }
    
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

void consumer(int rank)
{
    TrafficData td;
    unordered_map<int, int> car_counts;
    time_t current_hour = 0;
    
    cout << "   -Congestion in Descending Order-" << endl;
    cout << "      -Congestion for each Hour-" << endl
         << endl;

    auto start = chrono::high_resolution_clock::now();

    while (true)
    {
        MPI_Recv(&td, sizeof(TrafficData), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Check for termination signal
        if (td.timestamp == -1 && td.light_id == -1 && td.num_cars == -1)
            break;

        // current_hour = td.timestamp - (td.timestamp % 3600);
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
         << "Most Congested Hour: " << asctime(localtime(&max_congestion_hour)) << "Number of Cars Passed: " << max_cars_for_the_hour << " cars passed.\n" << endl;

    auto stop = high_resolution_clock::now(); // Stop time measurement
    auto duration = duration_cast<microseconds>(stop - start); // Calculate duration
    std::cout << ">> Time taken by function: " << duration.count() << " microseconds <<" << std::endl;
}

int main(int argc, char *argv[])
{
    const string data_file = "traffic_test_data.txt";
    ifstream infile(data_file);
    if (!infile.is_open())
    {
        cerr << "Error opening file: " << data_file << endl;
        return 1;
    }

    int num_processes, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        producer(infile, rank, num_processes);
    else
        consumer(rank);

    MPI_Finalize();
    return 0;

}