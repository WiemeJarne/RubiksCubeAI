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
	bool m_LayerOne{};

	DNA() = default;

	DNA(int turns, const std::string& ogScramble);

	DNA(int turns, const std::string& ogScramble, const std::string& genes, const CubeState& target);

	//calculate fitness score for DNA object
	void CalculateFitness(const CubeState& target);

	DNA Crossover(DNA partner);

	void Mutate(float mutationRate);

	static void SetGenerator(std::mt19937& generator) { m_Generator = generator; }

private:
	static std::mt19937 m_Generator;
};
#endif // !DNA_HPP