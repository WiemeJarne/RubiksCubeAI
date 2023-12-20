#include "GeneticAlgorithm.hpp"

GeneticAlgorithm::GeneticAlgorithm(int amountOfTurns, CubeState target, float mutationRate, int populationSize, std::mt19937 generator)
{
	m_Turns = amountOfTurns;
	m_OgScramble = target.GenerateScramble(generator);
	m_Target = target;
	m_MutationRate = mutationRate;

	for (int index{}; index < populationSize; ++index)
		m_Population.push_back(DNA(amountOfTurns, m_OgScramble, generator));

	CalculateFitness();

	DNA perfect{ DNA(amountOfTurns, m_OgScramble, generator) };
	perfect.m_Cube = CubeState();
	perfect.CalculateFitness(target);
	m_PerfectScore = perfect.m_Fitness;
}

void GeneticAlgorithm::CalculateFitness()
{
	for (DNA dna : m_Population)
		dna.CalculateFitness(m_Target);
}

void GeneticAlgorithm::NaturalSelection()
{
	m_MatingPool.clear();

	float maxFitness{};

	for (DNA dna : m_Population)
	{
		if (dna.m_Fitness > maxFitness)
			maxFitness = static_cast<float>(dna.m_Fitness);
	}

	for (DNA dna : m_Population)
	{
		float fitness{ static_cast<float>(dna.m_Fitness) };

		if (fitness != 0) fitness /= maxFitness;

		int amt{ static_cast<int>(fitness) * 100 }; //arbitrary multiplier of 100
		for (int index{}; index < amt; ++index)
			m_MatingPool.push_back(dna);
	}
}

void GeneticAlgorithm::Generate()
{
	DNA best{ GetBest() };
	m_Population[0] = best;

	DNA bestMutated{ DNA(m_Turns, m_OgScramble, best.m_Genes, m_Target, m_Generator) };

	bestMutated.Mutate(m_MutationRate);

	m_Population[1] = bestMutated;

	if (m_MatingPool.size() > 0u)
	{
		const int populationSize{ static_cast<int>(m_Population.size()) };
		const int matingPoolMaxIndex{ static_cast<int>(m_MatingPool.size()) - 1 };
		for (int index{ 2 }; index < populationSize; ++index)
		{
			DNA partnerA{ m_MatingPool[rand() % matingPoolMaxIndex] };
			DNA partnerB{ m_MatingPool[rand() % matingPoolMaxIndex] };
			DNA child = partnerA.Crossover(partnerB);
			child.Mutate(m_MutationRate);
			m_Population[index] = child;
		}
	}

	m_Population[0].CalculateFitness(m_Target);
	m_Population[1].CalculateFitness(m_Target);

	++m_Generations;
}

DNA GeneticAlgorithm::GetBest()
{
	int record{};
	int bestIndex{};

	const int populationSize{ static_cast<int>(m_Population.size()) };
	for (int index{}; index < populationSize; ++index)
	{
		if (m_Population[index].m_Fitness > record)
		{
			bestIndex = index;
			record = m_Population[index].m_Fitness;
		}
	}

	if (record == m_PerfectScore)
		m_Finnished = true;

	return m_Population[bestIndex];
}

void GeneticAlgorithm::AdjustRestictedTurns(int newRestictedTurns)
{
	for (DNA& dna : m_Population)
		dna.m_RestictedTurns = newRestictedTurns;
}

float GeneticAlgorithm::GetAverageFitness()
{
	float total{};

	for (DNA& dna : m_Population)
		total += dna.m_Fitness;

	return total / static_cast<float>(m_Population.size());
}