#ifndef DNA_HPP
#define DNA_HPP
#include "RubiksCube.hpp"

class DNA final
{
public:
	CubeState m_Cube{};
	int m_Fitness{};
	std::string m_OgScramble{};
	std::string m_Genes{};
	int m_Turns{};
	int m_RestictedTurns{};
	bool m_LayerOne{};
	bool m_LayerTwo{};
	std::mt19937 m_Generator{};

	DNA() = default;

	DNA(int turns, const std::string& ogScramble, std::mt19937& generator);

	DNA(int turns, const std::string& ogScramble, const std::string& genes, const CubeState& target, std::mt19937& generator);

	//calculate fitness score for DNA object
	void CalculateFitness(const CubeState& target);

	DNA Crossover(DNA partner);

	void Mutate(float mutationRate);

private:
};
#endif // !DNA_HPP