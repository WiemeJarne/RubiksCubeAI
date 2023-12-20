#ifndef DNA_HPP
#define DNA_HPP
#include "RubiksCube.hpp"

class DNA final
{
public:
	CubeState m_Cube{};
	int m_Fitness{};
	std::vector<CubeAction> m_OgScramble{};
	std::vector<CubeAction> m_Genes{};
	int m_Turns{};
	int m_RestictedTurns{};
	bool m_LayerOne{};
	bool m_LayerTwo{};
	std::mt19937 m_Generator{};

	DNA() = default;

	DNA(int turns, const std::vector<CubeAction>& ogScramble, std::mt19937& generator);

	DNA(int turns, const std::vector<CubeAction>& ogScramble, const std::vector<CubeAction>& genes, const CubeState& target, std::mt19937& generator);

	//calculate fitness score for DNA object
	void CalculateFitness(const CubeState& target);

	DNA Crossover(DNA partner);

	void Mutate(float mutationRate);

private:
};
#endif // !DNA_HPP