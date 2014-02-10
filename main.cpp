/* LANGUAGE: C++ */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <unordered_set>

const int num_cities_c = 25;

using namespace std;

void printTour(const vector<double> & tour);

// generates a tour randomly from [0, num_cities)
vector<double> genRandomTour(const int & num_cities)
{
	unordered_set<double> cities_generated;	
	vector<double> tour;
	tour.reserve(num_cities);

	srand(time(NULL));	

	while (tour.size() < num_cities)
	{
		int gen_city = rand() % num_cities;	

		// new city generated! add to both containers
		if (cities_generated.find(gen_city) == cities_generated.end())
		{
			cities_generated.insert(gen_city);
			tour.push_back(gen_city);
		}
	}		

	return tour;
}

vector<double> genTour(const int & num_cities)
{
	vector<double> tour(num_cities, -1);

	for (int i = 0; i < num_cities; i++)
		tour[i] = i;

	return tour;
}

/* objective function : cost of a tour */
double getTourCost(const vector<vector<double>> & costs, const vector<double> & tour)
{
	double totalCost = 0;
	cout << endl;

	// for each consecutive city i and j add distance between them to total cost
	for (int i = 0; i < tour.size() - 1; i++)
	{
		cout << i << " to " << i + 1 << " = " << costs[tour[i]][tour[i + 1]] << endl;
		totalCost += costs[tour[i]][tour[i + 1]];
	}

	// last city back to first city
	totalCost += costs[tour.back()][tour[0]];

	return totalCost;
}

// swap every ith and jth city 
deque<vector<double>> citySwapSuccessors(const vector<double> & tour)
{
	deque<vector<double>> successors;

	for (int i = 0; i < tour.size() - 1; i++)
	{
		for (int j = i + 1; j < tour.size(); j++)
		{
			vector<double> successor = tour;
			swap(successor[i], successor[j]);	
			successors.push_back(successor);
		}
	}

	return successors;
}

// reverse all substrings of length > 1 
vector<double> genSubstring(const vector<double> & tour, int sub_start, int sub_end);
deque<vector<double>> twoOptSuccessors(const vector<double> & tour)
{
	deque<vector<double>> successors;

	for (int i = 0; i < tour.size() - 1; i++)
		for (int j = i + 1; j < tour.size(); j++)	
			successors.push_back(genSubstring(tour, i, j));

	return successors;
}

vector<double> genSubstring(const vector<double> & tour, int sub_start, int sub_end)
{
	vector<double> successor = tour;

	while (sub_start < sub_end)
	{
		swap(successor[sub_start], successor[sub_end]);	
		++sub_start;
		--sub_end;
	}	

	return successor;
}

//vector<vector<double>> readCityMatrix(const string & file_name)
vector<vector<double>> readCityMatrix(ifstream & fin)
{
	if (!fin)
	{
		cout << "bad file passed to readCityMatrix function! exiting..." << endl;
		exit(1);
	}

	vector<vector<double>> matrix (num_cities_c, vector<double> (num_cities_c, -1));

	for (int i = 0; i < num_cities_c; i++)
	{
		for (int j = 0; j < num_cities_c; j++)
		{
			double val;
			fin >> val;

			if (!fin)
			{
				cerr << "error when reading file!" << endl;
				exit(1);
			}

			matrix[i][j] = val;

			// ignore commas
			fin.ignore(256, ',');

			// edge case that i'm too sick to deal with right now
			if (fin.eof())
				break;
		}
	}

	return matrix;
}

int main(int argc, char * argv[])
{	
	/* debugging purposes, prints successors using City Swap and 2-Opt successor functions for tour 1-2-3-4 
	vector<double> vec {1, 2, 3, 4};

	//auto res = citySwapSuccessors(vec);
	auto res = twoOptSuccessors(vec);

	for (const auto & each : res)
	{
		for (const auto & i : each)
			cout << i << " ";
		cout << endl;
	}
	*/

	// read-in cost symmetric matrix
	ifstream fin("cities.txt");
	auto costs = readCityMatrix(fin);
	fin.close();

	//* debugging purposes, prints cost matrix 
	cout << "cities.csv : " << endl;
	for (const auto & row : costs)
	{
		for (const auto & col : row)
			cout << setw(3) << col << " ";	

		cout << endl;
	}

	auto x = genTour(num_cities_c);
	auto y = genRandomTour(num_cities_c);	

	printTour(genTour(3));
	printTour(y);	

	cout << "(0, 1, 2) tour cost = " << getTourCost(costs, genTour(3)) << endl;

	return 0;
}

void printTour(const vector<double> & tour)
{
	for (const auto & city : tour)
		cout << city << " ";

	cout << endl;
}
