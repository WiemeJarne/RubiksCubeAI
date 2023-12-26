#include "DNA.hpp"

DNA::DNA(int turns, const std::string& ogScramble, std::mt19937& generator)
{
	m_Generator = generator;
	m_Cube = CubeState();
	m_Turns = turns;
	m_Genes = m_Cube.GenerateScramble(turns, generator);
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	m_Cube.Scramble(m_Genes.substr(0, turns - m_RestictedTurns));
	m_OgScramble = ogScramble;
}

DNA::DNA(int turns, const std::string& ogScramble, const std::string& genes, const CubeState& target, std::mt19937& generator)
{
	m_Cube = CubeState();
	m_Turns = turns;
	m_Genes = genes;
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	m_Cube.Scramble(m_Genes.substr(0, turns - m_RestictedTurns));
	m_OgScramble = ogScramble;
	CalculateFitness(target);
	m_Generator = generator;
}

//calculate fitness score for DNA object
void DNA::CalculateFitness(const CubeState& target)
{
	int score{};

	bool layerOne{ true };

	const int amountOfPieces{ static_cast<int>(target.pieces.size()) };

	const int amountOfColors{ static_cast<int>(target.pieces[0]->colors.size()) };

	for (int index{}; index < amountOfPieces; ++index)
	{
		bool equal{ true };

		for (int colorIndex{}; colorIndex < amountOfColors; ++colorIndex)
		{
			if (target.pieces[index]->colors[colorIndex] != m_Cube.pieces[index]->colors[colorIndex])
			{
				equal = false;
				continue;
			}
		}

		bool isInLayerOne{}; //= bottom layer
		if (index == 2 || index == 3 || index == 6 || index == 7)
			isInLayerOne = true;

		//if the piece is in bottom layer and the piece is not correct -> layerOne is false (not finnished)
		if (isInLayerOne && !equal)
			layerOne = false;

		if (equal && layerOne) ++score;
	}

	//put emphasis on building layer by layer
	if (layerOne)
		score *= 2;

	m_Fitness = static_cast<int>(pow(score, 3));

	m_LayerOne = layerOne;
}

DNA DNA::Crossover(DNA partner)
{
	DNA child{ DNA(m_Turns, m_OgScramble, m_Generator) };

	int midPoint{ static_cast<int>(m_Genes.size() / 2) };

	if (m_Genes[midPoint] == '\'')
		++midPoint;

	child.m_Genes = m_Genes.substr(0, midPoint);

	if (partner.m_Genes[midPoint] == '\'')
		--midPoint;

	child.m_Genes += partner.m_Genes.substr(midPoint, partner.m_Genes.size() - midPoint);

	child.m_Cube = CubeState();
	child.m_Cube.Scramble(child.m_OgScramble);
	//try to solve with genes
	child.m_Cube.Scramble(child.m_Genes.substr(0, m_Turns - m_RestictedTurns));

	return child;
}

void DNA::Mutate(float mutationRate)
{
	char rotations[] { 'F', 'B', 'U', 'D', 'L', 'R' };
	std::string newGenes{ m_Genes };
	
	for (int index{}; index < m_Genes.length() - m_RestictedTurns; ++index)
	{
		if (static_cast<float>(rand()) / RAND_MAX < mutationRate)
		{
			if (newGenes[index] != '\'')
				newGenes[index] = rotations[rand() % 6];
			else
			{
				char rotationsAdditional[]{ 'F', 'B', 'U', 'D', 'L', 'R', '\'' };
				if (index < newGenes.size() - 1 && newGenes[index - 1] != '\'' && newGenes[index + 1] != '\'')
					newGenes[index] = rotationsAdditional[rand() % 7];
			}
		}
	}

	m_Cube = CubeState();
	m_Genes = newGenes;
	m_Cube.Scramble(m_OgScramble);
	m_Cube.Scramble(m_Genes.substr(0, m_Turns - m_RestictedTurns));
}