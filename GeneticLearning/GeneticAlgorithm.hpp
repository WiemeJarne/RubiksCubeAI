#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP
#include "DNA.hpp"

class GeneticAlgorithm final
{
public:

	GeneticAlgorithm(int amountOfTurns, CubeState target, float mutationRate, int populationSize, std::mt19937& generator, const std::string& scramble = "");

	void CalculateFitness();
	//this function fills the mating pool for a new generation
	void NaturalSelection();
	void Generate(int amountOfTurns);
	DNA GetBest();
	int GetPerfectScore() { return m_PerfectScore; }
	int GetCurrentGenerationNr() { return m_CurrentGenerationNr; }
	bool GetIsFinnished() { return m_IsFinnished; }
	std::string& GetScramble() { return m_Scramble; }

private:
	int m_PerfectScore{};
	int m_CurrentGenerationNr{};
	float m_MutationRate{};
	bool m_IsFinnished{};
	CubeState m_Target{};
	std::string m_Scramble{};
	std::vector<DNA> m_Population{};
	std::vector<DNA> m_MatingPool{};
	std::mt19937 m_Generator{};
};
#endif // GENETIC_ALGORITHM_HPP