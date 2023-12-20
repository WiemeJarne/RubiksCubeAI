#include "DNA.hpp"

DNA::DNA(int turns, const std::vector<CubeAction>& ogScramble, std::mt19937& generator)
{
	m_Generator = generator;
	m_Cube = CubeState(false, generator);
	m_Turns = turns;
	m_Genes = m_Cube.GenerateScramble(generator, turns);
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	auto scrambleEndIt = m_Genes.begin() + turns - m_RestictedTurns;
	m_Cube.Scramble(std::vector<CubeAction>(m_Genes.begin(), scrambleEndIt));
	m_OgScramble = ogScramble;
}

DNA::DNA(int turns, const std::vector<CubeAction>& ogScramble, const std::vector<CubeAction>& genes, const CubeState& target, std::mt19937& generator)
{
	m_Cube = CubeState(false, generator);
	m_Turns = turns;
	m_Genes = genes;
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	auto scrambleEndIt = m_Genes.begin() + turns - m_RestictedTurns;
	m_Cube.Scramble(std::vector<CubeAction>(m_Genes.begin(), scrambleEndIt));
	m_OgScramble = ogScramble;
	CalculateFitness(target);
	m_Generator = generator;
}

//calculate fitness score for DNA object
void DNA::CalculateFitness(const CubeState& target)
{
	int score{};

	bool layerOne{ true };
	bool layerTwo{ true };

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
				break;
			}
		}

		bool isInLayerOne{}; //= bottom layer
		if (index == 2 || index == 3 || index == 6 || index == 7)
			isInLayerOne = true;

		//if the piece is not correct check in what layer it is
		if (!equal)
		{
			//is in bottom layer -> layerOne is false (not finnished) else the piece is in top layer (layerTwo) and that layer is not finnished
			if (isInLayerOne) layerOne = false;
			else layerTwo = false;
		}

		if (equal && layerOne) ++score;
		if (equal && isInLayerOne && !layerOne) ++score;
		if (equal && layerOne && layerTwo && !layerOne) ++score;
	}

	//put emphasis on building layer by layer
	if (layerOne)
	{
		score += 2;
		if (layerTwo) score += 2;
	}

	m_Fitness = static_cast<int>(pow(score, 3));

	m_LayerOne = layerOne;
	m_LayerTwo = layerTwo;
}

DNA DNA::Crossover(DNA partner)
{
	DNA child{ DNA(m_Turns, m_OgScramble, m_Generator) };

	int midPoint{ static_cast<int>(m_Genes.size() / 2) };

	child.m_Genes = std::vector<CubeAction>(m_Genes.begin(), m_Genes.begin() + midPoint);

	child.m_Genes.insert(child.m_Genes.end(), partner.m_Genes.begin() + midPoint, partner.m_Genes.end());

	child.m_Cube = CubeState(false, m_Generator);
	child.m_Cube.Scramble(child.m_OgScramble);
	//try to solve with genes
	child.m_Cube.Scramble(std::vector<CubeAction>(child.m_Genes.begin(), child.m_Genes.begin() + m_Turns - m_RestictedTurns));

	return child;
}

void DNA::Mutate(float mutationRate)
{
	std::vector<CubeAction> newGenes{};

	for (const CubeAction& action : m_Genes)
		newGenes.push_back(action);

	int startIndex{};
	if (m_LayerOne && m_LayerTwo)
		startIndex = m_Turns - m_RestictedTurns - 2;

	const int amountOfGenes{ static_cast<int>(m_Genes.size()) };

	for (int index{ startIndex }; index < amountOfGenes - m_RestictedTurns; ++index)
	{
		if (rand() / RAND_MAX < mutationRate)
		{
			newGenes[index] = CubeAction(m_Generator);
		}
	}

	m_Cube = CubeState(false, m_Generator);
	m_Genes = newGenes;
	m_Cube.Scramble(m_OgScramble);
	m_Cube.Scramble(std::vector<CubeAction>(m_Genes.begin(), m_Genes.begin() + m_Turns - m_RestictedTurns));
}