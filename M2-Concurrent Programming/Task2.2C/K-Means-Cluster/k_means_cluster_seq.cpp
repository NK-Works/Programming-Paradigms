/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*     This is a sequential K-Means Cluster algorithm.    */
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <limits>

using namespace std;
using namespace std::chrono;

// Structure to represent a point
struct Point
{
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

// Function to calculate Euclidean distance between two points
double calculateDistance(const Point &p1, const Point &p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Function to assign each point to the nearest centroid
void assignToClusters(const vector<Point> &points, const vector<Point> &centroids, vector<int> &assignments)
{
    for (size_t i = 0; i < points.size(); ++i)
    {
        double minDistance = numeric_limits<double>::max();
        int assignedCluster = -1;

        for (size_t j = 0; j < centroids.size(); ++j)
        {
            double distance = calculateDistance(points[i], centroids[j]);
            if (distance < minDistance)
            {
                minDistance = distance;
                assignedCluster = static_cast<int>(j);
            }
        }
        assignments[i] = assignedCluster;
    }
}

// Function to update centroid positions based on the points in each cluster
void updateCentroids(const vector<Point> &points, const vector<int> &assignments, vector<Point> &centroids)
{
    vector<int> clusterSizes(centroids.size(), 0);
    vector<double> sumX(centroids.size(), 0.0);
    vector<double> sumY(centroids.size(), 0.0);

    for (size_t i = 0; i < points.size(); ++i)
    {
        int cluster = assignments[i];
        clusterSizes[cluster]++;
        sumX[cluster] += points[i].x;
        sumY[cluster] += points[i].y;
    }
    for (size_t i = 0; i < centroids.size(); ++i)
    {
        if (clusterSizes[i] > 0)
        {
            centroids[i].x = sumX[i] / clusterSizes[i];
            centroids[i].y = sumY[i] / clusterSizes[i];
        }
    }
}

// K-Means clustering function with iterations and convergence check
void kMeans(const vector<Point> &points, int k, vector<Point> &centroids, int maxIterations, double convergenceThreshold)
{
    vector<int> assignments(points.size(), 0);
    vector<Point> oldCentroids; // Store old centroids for convergence check
    for (int i = 0; i < k; ++i)
    {
        centroids.push_back(points[rand() % points.size()]);
    }

    // Perform iterations
    for (int iteration = 0; iteration < maxIterations; ++iteration)
    {
        // Assign points to clusters
        assignToClusters(points, centroids, assignments);
        // Update centroids based on assigned points
        updateCentroids(points, assignments, centroids);
        // Check for convergence
        if (iteration > 0)
        {
            double maxCentroidChange = 0.0;
            for (size_t i = 0; i < centroids.size(); ++i)
            {
                double change = calculateDistance(centroids[i], oldCentroids[i]);
                maxCentroidChange = max(maxCentroidChange, change);
            }
            if (maxCentroidChange < convergenceThreshold)
            {
                cout << "Converged in " << iteration << " iterations." << endl;
                break;
            }
        }
        // Store current centroids for the next iteration
        oldCentroids = centroids;
    }
    cout << "---------------------------" << endl;
}

int main()
{
    // Example usage with a smaller dataset
    vector<Point> points;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> distribution(0.0, 100.0);
    
    for (int i = 0; i < 1000000; ++i)
    {
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
    for (size_t i = 0; i < centroids.size(); ++i)
    {
        cout << "Cluster " << i + 1 << ": (" << centroids[i].x << ", " << centroids[i].y << ")" << endl;
    }
    cout << "Time taken: " << duration.count() << " microseconds." << endl;
    return 0;
}
