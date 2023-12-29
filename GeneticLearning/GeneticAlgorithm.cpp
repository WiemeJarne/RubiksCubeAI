#include "GeneticAlgorithm.hpp"

GeneticAlgorithm::GeneticAlgorithm(int amountOfTurns, CubeState target, float mutationRate, int populationSize, std::mt19937& generator, const std::string& scramble)
{
	if (scramble != "")
		m_Scramble = target.GenerateScramble(amountOfTurns, generator);
	else m_Scramble = scramble;

	m_Target = target;
	m_MutationRate = mutationRate;

	m_Population.resize(populationSize);

	for (int index{}; index < populationSize; ++index)
		m_Population[index] = DNA(amountOfTurns, m_Scramble);

	CalculateFitness();

	DNA perfect{ DNA(amountOfTurns, m_Scramble) };
	perfect.m_Cube = CubeState();
	perfect.CalculateFitness(target);
	m_PerfectScore = perfect.m_Fitness;
}

void GeneticAlgorithm::CalculateFitness()
{
	for (DNA& dna : m_Population)
		dna.CalculateFitness(m_Target);
}

void GeneticAlgorithm::NaturalSelection()
{
	m_MatingPool.clear();

	float maxFitness{};

	//find the maxFitness
	for (const DNA& dna : m_Population)
	{
		if (dna.m_Fitness > maxFitness)
			maxFitness = static_cast<float>(dna.m_Fitness);
	}

	for (const DNA& dna : m_Population)
	{
		float fitness{ static_cast<float>(dna.m_Fitness) };

		if (fitness != 0) fitness /= maxFitness;

		int amt{ static_cast<int>(fitness) * 100 }; //arbitrary multiplier of 100
		for (int index{}; index < amt; ++index)
			m_MatingPool.push_back(dna);
	}
}

void GeneticAlgorithm::Generate(int amountOfTurns)
{
	DNA best{ GetBest() };
	m_Population[0] = best;

	DNA bestMutated{ DNA(amountOfTurns, m_Scramble, best.m_Genes, m_Target) };

	bestMutated.Mutate(m_MutationRate);

	m_Population[1] = bestMutated;

	const int populationSize{ static_cast<int>(m_Population.size()) };
	std::uniform_int_distribution<unsigned int> dist(0, static_cast<int>(m_MatingPool.size()) - 1);
	for (int index{ 2 }; index < populationSize; ++index)
	{
		DNA partnerA{ m_MatingPool[dist(m_Generator)] };
		DNA partnerB{ m_MatingPool[dist(m_Generator)] };
		DNA child = partnerA.Crossover(partnerB);
		child.Mutate(m_MutationRate);
		m_Population[index] = child;
	}

	m_Population[0].CalculateFitness(m_Target);
	m_Population[1].CalculateFitness(m_Target);

	++m_CurrentGenerationNr;
}

DNA GeneticAlgorithm::GetBest()
{
	int highestFitnesss{};
	int bestIndex{};

	const int populationSize{ static_cast<int>(m_Population.size()) };
	for (int index{}; index < populationSize; ++index)
	{
		if (m_Population[index].m_Fitness > highestFitnesss)
		{
			bestIndex = index;
			highestFitnesss = m_Population[index].m_Fitness;
		}
	}

	if (highestFitnesss == m_PerfectScore)
		m_IsFinnished = true;

	return m_Population[bestIndex];
}