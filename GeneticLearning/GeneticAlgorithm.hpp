#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP
#include "DNA.hpp"

class GeneticAlgorithm final
{
public:
	DNA m_PreviousBest{};
	float m_MutationRate{};
	std::vector<DNA> m_Population{};
	std::vector<DNA> m_MatingPool{};
	CubeState m_Target{};
	std::string m_OgScramble{};
	int m_Generations{};
	bool m_Finnished{};
	int m_FitnessPow{ 3 };
	int m_PerfectScore{};
	int m_Turns{};
	std::mt19937 m_Generator{};

	GeneticAlgorithm(int amountOfTurns, CubeState target, float mutationRate, int populationSize, std::mt19937 generator);

	void CalculateFitness();
	//this function fills the mating pool for a new generation
	void NaturalSelection();
	void Generate();
	DNA GetBest();
	void AdjustRestictedTurns(int newRestictedTurns);
	float GetAverageFitness();
};
#endif // !GENETIC_ALGORITHM_HPP