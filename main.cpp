#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>

const int num_cities_c = 25;

using namespace std;

// swap every ith and jth city 
deque<vector<int>> citySwapSuccessors(const vector<int> & tour)
{
	deque<vector<int>> successors;

	for (int i = 0; i < tour.size() - 1; i++)
	{
		for (int j = i + 1; j < tour.size(); j++)
		{
			vector<int> successor = tour;
			swap(successor[i], successor[j]);	
			successors.push_back(successor);
		}
	}

	return successors;
}

// reverse all substrings of length > 1 
deque<vector<int>> twoOptSuccessors(const vector<int> & tour)
{
	deque<vector<int>> successors;

	for (int i = 2; i < tour.size(); i++)
	{
		
	}

	return successors;
}

vector<vector<int>> readCityMatrix(const string & file_name)
{
	ifstream fin(file_name);

	if (!fin)
	{
		cerr << "error on opening file: '" << file_name << "'" << endl;
		exit(1);
	}

	vector<vector<int>> matrix (num_cities_c, vector<int> (num_cities_c, 0));

	for (int i = 0; i < num_cities_c; i++)
	{
		for (int j = 0; j < num_cities_c; j++)
		{
			int val;
			fin >> val;

			if (!fin)
			{
				cerr << "error on opening file: '" << file_name << "'" << endl;
				exit(1);
			}

			matrix[i][j] = val;
		}
	}

	return matrix;
}

int main(int argc, char * argv[])
{	
	vector<int> vec {1, 2, 3, 4};

	//auto res = citySwapSuccessors(vec);
	auto res = twoOptSuccessors(vec);

	for (const auto & each : res)
	{
		for (const auto & i : each)
			cout << i << " ";
		cout << endl;
	}

	auto matrix = readCityMatrix("cities.csv");

	for (const auto & row : matrix)
	{
		for (const auto & col : row)
		{
			cout << col << " ";	
		}	

		cout << endl;
	}

	return 0;
}
