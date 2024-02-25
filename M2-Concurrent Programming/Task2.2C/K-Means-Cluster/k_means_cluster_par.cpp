/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*       This is a pthread K-Means Cluster algorithm.     */
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <limits>
#include <pthread.h>
#include <thread>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS thread::hardware_concurrency()

// Structure to represent a point
struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

// Structure to pass data to threads
struct ThreadData {
    const vector<Point>* points;
    const vector<int>* assignments;
    vector<Point>* centroids;
    vector<int> clusterSizes;
    vector<double> sumX;
    vector<double> sumY;
    size_t start;
    size_t end;
};

// Function to calculate Euclidean distance between two points
double calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Function to assign each point to the nearest centroid
void assignToClusters(const vector<Point>& points, const vector<Point>& centroids, vector<int>& assignments) {
    for (size_t i = 0; i < points.size(); ++i) {
        double minDistance = numeric_limits<double>::max();
        int assignedCluster = -1;

        for (size_t j = 0; j < centroids.size(); ++j) {
            double distance = calculateDistance(points[i], centroids[j]);
            if (distance < minDistance) {
                minDistance = distance;
                assignedCluster = static_cast<int>(j);
            }
        }
        assignments[i] = assignedCluster;
    }
}

// Thread function to update centroid positions based on the points in each cluster
void* updateCentroidsThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);

    for (size_t i = data->start; i < data->end; ++i) {
        int cluster = (*data->assignments)[i];
        data->clusterSizes[cluster]++;
        data->sumX[cluster] += (*data->points)[i].x;
        data->sumY[cluster] += (*data->points)[i].y;
    }

    return nullptr;
}

// Modified K-Means function to parallelize centroid updates
void kMeans(const vector<Point>& points, int k, vector<Point>& centroids, int maxIterations, double convergenceThreshold) {
    vector<int> assignments(points.size(), 0);
    vector<Point> oldCentroids; // Store old centroids for convergence check
    vector<int> clusterSizes(k, 0);
    vector<double> sumX(k, 0.0);
    vector<double> sumY(k, 0.0);

    pthread_t* threads = new pthread_t[NUM_THREADS];
    ThreadData* threadData = new ThreadData[NUM_THREADS];

    // Initialize centroids randomly
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> distribution(0, points.size() - 1);

    for (int i = 0; i < k; ++i) {
        centroids.push_back(points[distribution(gen)]);
    }

    // Perform iterations
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        // Assign points to clusters
        assignToClusters(points, centroids, assignments);

        // Initialize thread data and divide tasks among threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            size_t start = i * (points.size() / NUM_THREADS);
            size_t end = (i + 1) * (points.size() / NUM_THREADS);
            threadData[i] = {&points, &assignments, &centroids, clusterSizes, sumX, sumY, start, end};
        }

        // Create threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], nullptr, updateCentroidsThread, &threadData[i]);
        }

        // Join threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], nullptr);
        }

        // Combine results from threads
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < NUM_THREADS; ++j) {
                clusterSizes[i] += threadData[j].clusterSizes[i];
                sumX[i] += threadData[j].sumX[i];
                sumY[i] += threadData[j].sumY[i];
            }
        }

        // Update centroids based on assigned points
        for (size_t i = 0; i < centroids.size(); ++i) {
            if (clusterSizes[i] > 0) {
                centroids[i].x = sumX[i] / clusterSizes[i];
                centroids[i].y = sumY[i] / clusterSizes[i];
            }
        }

        // Check for convergence
        if (iteration > 0) {
            double maxCentroidChange = 0.0;
            for (size_t i = 0; i < centroids.size(); ++i) {
                double change = calculateDistance(centroids[i], oldCentroids[i]);
                maxCentroidChange = max(maxCentroidChange, change);
            }
            if (maxCentroidChange < convergenceThreshold) {
                cout << "Converged in " << iteration << " iterations." << endl;
                break;
            }
        }

        // Store current centroids for the next iteration
        oldCentroids = centroids;
    }

    cout << "---------------------------" << endl;

    // Clean up
    delete[] threads;
    delete[] threadData;
}

int main() {
    // Example usage with a smaller dataset
    vector<Point> points;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> distribution(0.0, 100.0);
    for (int i = 0; i < 1000000; ++i) {
        points.emplace_back(distribution(gen), distribution(gen));
    }
     cout << "|Running for size 1000000|" << endl;
    vector<Point> centroids;
    int k = 2; // Number of clusters
    int maxIterations = 100;
    double convergenceThreshold = 0.001;

    // Measure the time taken
    auto start = high_resolution_clock::now();

    kMeans(points, k, centroids, maxIterations, convergenceThreshold);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    // Print the final centroids
    cout << "     -Final Centroids-" << endl;
    for (size_t i = 0; i < centroids.size(); ++i) {
        cout << "Cluster " << i + 1 << ": (" << centroids[i].x << ", " << centroids[i].y << ")" << endl;
    }
    cout << "Time taken: " << duration.count() << " microseconds." << endl;

    return 0;
}
