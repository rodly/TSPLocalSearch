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

using namespace std;

const int num_cities_c = 25;

struct TourData
{
	double avg_cost,
		   lowest_cost,
		   runs_below_threshold;

	vector<double> optimal_tour;  
};

TourData steepestDescentLocalSearch(const vector<vector<double>> & costs, vector<double> tour, deque<vector<double>> (*successor_func)(const vector<double> &), const int & num_runs, const int & num_random_restarts, const double & threshold);
void printTour(const vector<double> & tour);
vector<double> genRandomTour(const int & num_cities);
vector<double> genTour(const int & num_cities);
deque<vector<double>> citySwapSuccessors(const vector<double> & tour);
deque<vector<double>> twoOptSuccessors(const vector<double> & tour);
vector<double> genSubstring(const vector<double> & tour, int sub_start, int sub_end);
vector<vector<double>> readCityMatrix(ifstream & fin);
void printCostMatrix(const vector<vector<double>> & costs);
vector<double> getOptimalTour(const vector<vector<double>> & costs, const deque<vector<double>> & tours);
double getTourCost(const vector<vector<double>> & costs, const vector<double> & tour);

int main(int argc, char * argv[])
{	
	srand(time(NULL));	

	// read-in cost symmetric matrix
	ifstream fin("cities.txt");
	auto costs = readCityMatrix(fin);
	fin.close();

	//printCostMatrix();

	// A. 
	const auto std_tour = genTour(num_cities_c);	
	printTour(std_tour);
	cout << "cost: " << getTourCost(costs, std_tour) << endl;

	// B. 10,000 random tour stats
	cout << endl;	
	const int sample_size_c = 10000;	
	double lowest_cost = numeric_limits<double>::max();	
	double average_cost = 0;

	for (int i = 0; i < sample_size_c; i++)
	{
		auto cur_tour = genRandomTour(num_cities_c);
		double tour_cost = getTourCost(costs, cur_tour);

		if (tour_cost < lowest_cost)
			lowest_cost = tour_cost;

		average_cost += tour_cost;
	}

	average_cost /= sample_size_c;	
	cout <<	sample_size_c << " random tours: " << endl;
	cout << "sample size: " << sample_size_c << ", lowest tour cost: " << lowest_cost << ", average cost: " << average_cost << endl;

	// C. City Swap successor function on tour [0, 24] 
	cout << endl;	
	auto city_swap_successors = citySwapSuccessors(std_tour);
	auto tour = getOptimalTour(costs, city_swap_successors);

	printTour(std_tour);
	cout << "has optimal City Swap successor: " << endl;
	printTour(tour);	
	cout << "with tour cost: " << lowest_cost << endl;

	// D. 2-Opt successor function on tour [0, 24] 
	cout << endl;	
	auto two_opt_successors = twoOptSuccessors(std_tour);
	tour = getOptimalTour(costs, two_opt_successors);

	printTour(std_tour);
	cout << "has optimal 2-Opt successor: " << endl;
	printTour(tour);	
	cout << "with tour cost: " << lowest_cost << endl;

	// E. 50 iterations of City Swap hill-climbing local search
	cout << endl;	
	TourData td = steepestDescentLocalSearch(costs, std_tour, citySwapSuccessors, 1, 1, 0);

	cout << "50 iterations starting with initial tour [0, 24] (City Swap successors) has optimal tour: " << endl;
	printTour(td.optimal_tour);
	cout << "with cost: " << getTourCost(costs, td.optimal_tour) << endl;

	// F. i = 1 random restart, ii = 10 random restarts 
	cout << endl;	
	int num_restarts = 1;	
	const int num_runs = 100;
	const int threshold = 320;

	td = steepestDescentLocalSearch(costs, genRandomTour(num_cities_c), citySwapSuccessors, num_runs, num_restarts, threshold);
	cout << "restarts = " << num_restarts << ", # of runs below 320 = " << td.runs_below_threshold << endl;

	num_restarts = 10;

	td = steepestDescentLocalSearch(costs, genRandomTour(num_cities_c), citySwapSuccessors, num_runs, num_restarts, threshold);
	cout << "restarts = " << num_restarts << ", # of runs below 320 = " << td.runs_below_threshold << endl;

	// H.
	cout << "10 random restarts, avg cost using City Swap successor function = " << td.avg_cost << endl;

	// I. 
	td = steepestDescentLocalSearch(costs, genRandomTour(num_cities_c), twoOptSuccessors, num_runs, num_restarts, threshold);
	cout << "10 random restarts, avg cost using 2-Opt successor function = " << td.avg_cost << endl;

	return 0;
}

// returns tour data for num_runs with num_random_restarts on the initial 'tour' given a successor function
TourData steepestDescentLocalSearch(const vector<vector<double>> & costs, vector<double> tour, deque<vector<double>> (*successor_func)(const vector<double> &), const int & num_runs, const int & num_random_restarts, const double & threshold)
{
	TourData td;
	td.lowest_cost = numeric_limits<double>::max();
	const int iters_c = 50;
	bool below_threshold = false;

	for (int run = 0; run < num_runs; run++)
	{
		below_threshold = false;

		for (int cur_restart = 0; cur_restart < num_random_restarts; cur_restart++)
		{
			tour = genRandomTour(num_cities_c);

			for (int iter = 0; iter < iters_c; iter++)
			{
				/* run 50 iterations of getting successors for a given tour, 
				 * finding the optimal tour in each of those, 
				 * add to an avg var and keep track of below threshold runs and overall lowest cost tracking */

				auto successors = successor_func(tour);			
				tour = getOptimalTour(costs, successors);

				const auto tour_cost = getTourCost(costs, tour);

				// threshold per run tracking	
				if (!below_threshold && tour_cost < threshold)
				{
					td.runs_below_threshold++;
					below_threshold = true;
				}

				// lowest cost tracking
				if (tour_cost < td.lowest_cost)
				{
					td.lowest_cost = tour_cost;	
					td.optimal_tour = tour;
				}
			}
		}

		td.avg_cost += td.lowest_cost;
	}
	
	td.avg_cost /= num_runs;
	return td;
}

void printTour(const vector<double> & tour)
{
	for (const auto & city : tour)
		cout << city << " ";

	cout << endl;
}

// generates a tour randomly from [0, num_cities)
vector<double> genRandomTour(const int & num_cities)
{
	unordered_set<double> cities_generated;	
	vector<double> tour;
	tour.reserve(num_cities);

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

	// for each consecutive city i and j add distance between them to total cost
	for (int i = 0; i < tour.size() - 1; i++)
		totalCost += costs[tour[i]][tour[i + 1]];

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

			// consume non-digits
			while (isspace(fin.peek()) || fin.peek() == ',')
				fin.get();

			// edge case that i'm too sick to deal with right now
			if (fin.eof())
				break;
		}
	}

	return matrix;
}

void printCostMatrix(const vector<vector<double>> & costs)
{
	cout << "cities.csv : " << endl;
	for (const auto & row : costs)
	{
		for (const auto & col : row)
			cout << setw(3) << col << " ";	

		cout << endl;
	}
}

vector<double> getOptimalTour(const vector<vector<double>> & costs, const deque<vector<double>> & tours)
{
	const vector<double> * optimal_tour;
	double lowest_cost = numeric_limits<double>::max();

	for (const auto & tour : tours)
	{
		auto tour_cost = getTourCost(costs, tour);

		if (tour_cost < lowest_cost)
		{
			optimal_tour = &tour;
			lowest_cost = tour_cost;
		}
	}

	return *optimal_tour;
}
