/* This code is made by Anneshu Nag, Student ID-2210994760*/
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <chrono>

using namespace std;

int main()
{
    const int num_lights = 5;
    const int num_measurements = 12;
    ofstream outfile("traffic_test_data.txt");
    
    // Write header fields to the file
    outfile << "Date-Time, Signal_ID, Cars_Passed\n";
    
    // Get the current date
    auto current_time = chrono::system_clock::now();
    time_t current_time_t = chrono::system_clock::to_time_t(current_time);
    tm *current_tm = localtime(&current_time_t);

    // Generate data for each hour
    for (int hour = 0; hour < 24; hour++)
    {
        // Generate data for each 5-minute interval
        for (int i = 0; i < num_measurements; i++)
        {
            // Generate data for each traffic light
            for (int light_id = 1; light_id <= num_lights; light_id++)
            {
                // Generate random number of cars passed by
                default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());
                normal_distribution<double> distribution(50, 20);
                double num_cars = distribution(generator);
                num_cars = max(num_cars, 0.0);
                num_cars = round(num_cars);

                // Set the timestamp using the current date and the generated time
                tm time_info = *current_tm;
                time_info.tm_hour = hour;
                time_info.tm_min = i * 5;
                time_info.tm_sec = 0;
                time_t timestamp = mktime(&time_info);

                // Print data to file
                outfile << timestamp << ", " << light_id << ", " << static_cast<int>(num_cars) << "\n";
            }
        }
    }

    outfile.close();
    cout << "Data file generated: traffic_test_data.txt" << endl;
    return 0;
}