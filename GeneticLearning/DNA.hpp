#ifndef DNA_HPP
#define DNA_HPP
#include "RubiksCube.hpp"

class DNA final
{
public:
	DNA() = default;

	DNA(int turns, const std::string& ogScramble);

	DNA(int turns, const std::string& ogScramble, const std::string& genes, const CubeState& target);

	//calculate fitness score for DNA object
	void CalculateFitness(const CubeState& target);

	DNA Crossover(DNA partner);

	void Mutate(float mutationRate);

	static void SetGenerator(std::mt19937& generator) { m_Generator = generator; }
	
	int GetTurns() const { return m_Turns; }
	int GetFitness() const { return m_Fitness; }
	const std::string& GetGenes() const { return m_Genes; }
	const CubeState& GetCubeState() const { return m_Cube; }
	
	void SetCubeState(const CubeState& cubeState) { m_Cube = cubeState; }

private:
	int m_Turns{};
	int m_Fitness{};
	std::string m_Scramble{};
	std::string m_Genes{};
	CubeState m_Cube{};

	static std::mt19937 m_Generator;
};
#endif // !DNA_HPP