#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

using std::string;

void randomizePath(std::vector<int> &path, int numberOfCities);
int calculateDistance(int numberOfCities, std::vector<int> path, std::vector<std::vector<int>> distanceMap);
void mutate(std::vector<int> &path, int numberOfCities);
std::vector<int> mate(std::vector<int> &path1, std::vector<int> &path2, int numberOfCities);
void rank(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int population);
void r_merge_sort(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int begin, int end);
void r_merge(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int begin, int end);
std::pair<int, int> chooseParents(int population);
int weightedRand(int population);

std::random_device rd; // a seed source for the random number engine
std::mt19937 gen(rd());

int main()
{
    const int population = 500;
    srand(std::time(0));

    std::cout << "Please input TSP file: ";
    string filename;
    std::cin >> filename;
    std::ifstream inputFile(filename);
    while (!inputFile.is_open())
    {
        std::cout << "Please input a valid TSP file: ";
        std::cin >> filename;
        inputFile.open(filename);
    }

    int numberOfCities{0};
    inputFile >> numberOfCities;

    std::vector<std::string> cities(numberOfCities);
    for (int i = 0; i < numberOfCities; i++)
    {
        inputFile >> cities[i];
    }

    std::vector<std::vector<int>> distanceMap(numberOfCities, std::vector<int>(numberOfCities, 0));
    for (int i = 0; i < numberOfCities; i++)
    {
        for (int j = 0; j < numberOfCities; j++)
        {
            inputFile >> distanceMap[i][j];
        }
    }

    std::cout << "\nCities: \n";
    for (int i = 0; i < numberOfCities; i++)
    {
        std::cout << cities[i] << '\n';
    }

    std::cout << "\nDistance map:\n";

    for (int i = 0; i < numberOfCities; i++)
    {
        for (int j = 0; j < numberOfCities; j++)
        {
            std::cout << distanceMap[i][j] << " ";
        }
        std::cout << '\n';
    }

    int totalDistance = 0;

    std::vector<int> citiesPool(numberOfCities);
    for (int i = 0; i < numberOfCities; i++)
    {
        citiesPool[i] = i;
    }

    std::vector<int> path(numberOfCities);
    for (int i = 0; i < numberOfCities; i++)
    {
        path[i] = i;
    }

    std::vector<int> path2(numberOfCities);
    for (int i = 0; i < numberOfCities; i++)
    {
        path2[i] = i;
    }

    randomizePath(path, numberOfCities);
    randomizePath(path2, numberOfCities);
    std::vector<int> newPath = mate(path, path2, numberOfCities);

    std::cout << "\nPath 1: \n";
    for (int i = 0; i < numberOfCities; i++)
    {
        std::cout << path[i] << " ";
    }

    std::cout << "\nPath 2: \n";
    for (int i = 0; i < numberOfCities; i++)
    {
        std::cout << path2[i] << " ";
    }

    std::cout << "\nNew Path: \n";
    for (int i = 0; i < numberOfCities; i++)
    {
        std::cout << newPath[i] << " ";
    }

    mutate(newPath, numberOfCities);

    std::cout << "\nMutated Path: \n";
    for (int i = 0; i < numberOfCities; i++)
    {
        std::cout << newPath[i] << " ";
    }

    totalDistance = calculateDistance(numberOfCities, path, distanceMap);
    std::cout << "\nTotal distance: " << totalDistance << "\n";

    for (int i = 0; i < numberOfCities; i++)
    {
        path[i] = i;
    }

    std::vector<std::vector<int>> pathPopulation(population, path);
    std::vector<int> totalDistanceArray(population, 0);

    std::cout << "Starting to generate...\n";

    for (int i = 0; i < population; i++)
    {
        randomizePath(pathPopulation[i], numberOfCities);
        totalDistanceArray[i] = calculateDistance(numberOfCities, pathPopulation[i], distanceMap);
    }

    rank(pathPopulation, totalDistanceArray, population);

    int remainingGeneration = 100000;
    int currentGeneration = 0;
    int eliminationPortion = population * 0.6;
    int mutatePortion = population * 0.95;
    int bestOfLastGeneration = totalDistanceArray[population - 1];
    std::vector<std::vector<int>> newbornPopulation(population - eliminationPortion);

    // int arr[100] = {0};

    while (remainingGeneration > 0)
    {
        currentGeneration++;
        // Generate specimen
        if (totalDistanceArray[population - 1] < bestOfLastGeneration)
        {
            std::cout << "Best of generation: " << currentGeneration << "\n";
            std::cout << "Path: [";
            for (int i = 0; i < numberOfCities; i++)
            {
                std::cout << pathPopulation[population - 1][i] << " ";
            }
            std::cout << "] - ";
            std::cout << "Distance: " << totalDistanceArray[population - 1] << "\n";
            bestOfLastGeneration = totalDistanceArray[population - 1];
        }

        for (int i = 0; i < population - eliminationPortion; i++)
        {
            // std::cout << "Choose partent\n";
            std::pair<int, int> parents = chooseParents(population);
            // std::cout << "Mateing\n";
            newbornPopulation[i] = mate(pathPopulation[parents.first], pathPopulation[parents.second], numberOfCities);
        }

        for (int i = 0; i < mutatePortion; i++)
        {
            mutate(pathPopulation[i], numberOfCities);
        }

        for (int i = 0; i < population - eliminationPortion; i++)
        {
            pathPopulation[i] = newbornPopulation[i];
            totalDistanceArray[i] = calculateDistance(numberOfCities, pathPopulation[i], distanceMap);
        }

        rank(pathPopulation, totalDistanceArray, population);
        remainingGeneration--;

        if (remainingGeneration == 0)
        {
            std::cout << "Do you want to continue the simulation? (Y/N - default) ";
            char input;
            std::cin >> input;
            if (input == 'y' || input == 'Y')
            {
                remainingGeneration += 100000;
            }
        }
    }

    // for (int i = 0; i < 100; i++)
    // {
    //     std::cout << "Occurence of " << i << " is " << arr[i] << "\n";
    //     remainingGeneration += arr[i];
    // }

    inputFile.close();
    return 0;
}

void randomizePath(std::vector<int> &path, int numberOfCities)
{
    for (int i = numberOfCities - 1; i > 0; i--)
    {
        int randIndex = rand() % (i + 1);
        int temp = path[i];
        path[i] = path[randIndex];
        path[randIndex] = temp;
    }
}

int calculateDistance(int numberOfCities, std::vector<int> path, std::vector<std::vector<int>> distanceMap)
{
    int totalDistance = 0;
    for (int i = 0; i < numberOfCities - 1; i++)
    {
        totalDistance += distanceMap[path[i]][path[i + 1]];
    }
    totalDistance += distanceMap[path[numberOfCities - 1]][path[0]];

    return totalDistance;
}

std::vector<int> mate(std::vector<int> &path1, std::vector<int> &path2, int numberOfCities)
{
    int startIndex = rand() % numberOfCities;
    int endIndex = startIndex + rand() % (numberOfCities - startIndex);

    std::vector<int> newPath(numberOfCities);
    std::set<int> hasImported;

    for (int i = startIndex; i <= endIndex; i++)
    {
        newPath[i] = path1[i];
        hasImported.insert(newPath[i]);
    }

    // std::cout << "Start index: " << startIndex << " end index: " << endIndex << "\n";

    int secondPathIndex = 0;

    for (int i = 0; i < startIndex;)
    {
        // std::cout << secondPathIndex << " ";
        if (hasImported.find(path2[secondPathIndex]) == hasImported.end())
        {
            newPath[i] = path2[secondPathIndex];
            // std::cout << path2[secondPathIndex] << "\n";
            i++;
        }
        secondPathIndex++;
    }

    for (int i = endIndex + 1; i < numberOfCities;)
    {
        // std::cout << secondPathIndex << " ";
        if (hasImported.find(path2[secondPathIndex]) == hasImported.end())
        {
            newPath[i] = path2[secondPathIndex];
            // std::cout << path2[secondPathIndex] << "\n";
            i++;
        }
        secondPathIndex++;
    }
    return newPath;
}

void mutate(std::vector<int> &path, int numberOfCites)
{
    int possibleMutate = numberOfCites / 1.2f;
    for (int i = 0; i < possibleMutate; i++)
    {
        int firstIndex = rand() % numberOfCites;
        int secondIndex = firstIndex + rand() % (numberOfCites - firstIndex);
        int temp = path[firstIndex];
        path[firstIndex] = path[secondIndex];
        path[secondIndex] = temp;
    }
}

void rank(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int population)
{
    r_merge_sort(path, totalDistance, 0, population - 1);
}

void r_merge_sort(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int begin, int end)
{
    if (begin >= end)
    {
        return;
    }

    int mid = (begin + end) / 2;
    r_merge_sort(path, totalDistance, begin, mid);
    r_merge_sort(path, totalDistance, mid + 1, end);
    r_merge(path, totalDistance, begin, end);
}

void r_merge(std::vector<std::vector<int>> &path, std::vector<int> &totalDistance, int begin, int end)
{
    // std::cout << "Begin: " << begin << " End: " << end << "\n";
    int mid = (begin + end) / 2;
    int subArrLeft = mid - begin + 1;
    int subArrRight = end - mid;

    std::vector<int> distanceLeft(subArrLeft);
    std::vector<int> distanceRight(subArrRight);
    std::vector<std::vector<int>> pathLeft(subArrLeft);
    std::vector<std::vector<int>> pathRight(subArrRight);

    for (int i = 0; i < subArrLeft; i++)
    {
        distanceLeft[i] = totalDistance[begin + i];
        pathLeft[i] = path[begin + i];
    }

    for (int i = 0; i < subArrRight; i++)
    {
        distanceRight[i] = totalDistance[mid + i + 1];
        pathRight[i] = path[mid + i + 1];
    }

    int leftIndex = 0;
    int rightIndex = 0;
    int mergedIndex = begin;

    while (leftIndex < subArrLeft && rightIndex < subArrRight)
    {
        // std::cout << "Left: " << distanceLeft[leftIndex] << " Right: " << distanceRight[rightIndex] << '\n';
        // std::cout << "MergeIndex: " << mergedIndex << "\n";
        if (distanceLeft[leftIndex] < distanceRight[rightIndex])
        {
            totalDistance[mergedIndex] = distanceRight[rightIndex];
            path[mergedIndex] = pathRight[rightIndex];
            rightIndex++;
        }
        else
        {
            totalDistance[mergedIndex] = distanceLeft[leftIndex];
            path[mergedIndex] = pathLeft[leftIndex];
            leftIndex++;
        }
        // std::cout << "Total Distance[" << mergedIndex << "]: " << totalDistance[mergedIndex] << "\n";
        mergedIndex++;
    }

    while (leftIndex < subArrLeft)
    {
        // std::cout << "MergeIndex: " << mergedIndex << "\n";
        totalDistance[mergedIndex] = distanceLeft[leftIndex];
        path[mergedIndex] = pathLeft[leftIndex];
        // std::cout << "Total Distance[" << mergedIndex << "]: " << totalDistance[mergedIndex] << "\n";
        leftIndex++;
        mergedIndex++;
    }
    while (rightIndex < subArrRight)
    {
        // std::cout << "MergeIndex: " << mergedIndex << "\n";
        totalDistance[mergedIndex] = distanceRight[rightIndex];
        path[mergedIndex] = pathRight[rightIndex];
        // std::cout << "Total Distance[" << mergedIndex << "]: " << totalDistance[mergedIndex] << "\n";
        rightIndex++;
        mergedIndex++;
    }
}

std::pair<int, int> chooseParents(int population)
{
    int firstParent{weightedRand(population)};
    int secondParent{weightedRand(population)};
    while (secondParent == firstParent)
    {
        secondParent = weightedRand(population);
    }
    return std::pair<int, int>(firstParent, secondParent);
}

int weightedRand(int population)
{
    long long int weightedTotal = population * (population + 1) / 2;

    std::uniform_int_distribution<int> distrib(0, weightedTotal - 1);
    int randomInt = distrib(gen);
    for (int i = 0; i < population; i++)
    {
        randomInt -= (i + 1);
        if (randomInt < 0)
        {
            return i;
        }
    }
    return -1;
}