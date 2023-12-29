#include "DNA.hpp"

std::mt19937 DNA::m_Generator;

DNA::DNA(int turns, const std::string& ogScramble)
{
	m_Cube = CubeState();
	m_Turns = turns;
	m_Genes = m_Cube.GenerateScramble(turns, m_Generator);
	//initial scramble
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	m_Cube.Scramble(m_Genes);
	m_Scramble = ogScramble;
}

DNA::DNA(int turns, const std::string& ogScramble, const std::string& genes, const CubeState& target)
{
	m_Cube = CubeState();
	m_Turns = turns;
	m_Genes = genes;
	m_Cube.Scramble(ogScramble);
	//try to solve with genes
	m_Cube.Scramble(m_Genes.substr(0, turns));
	m_Scramble = ogScramble;
	CalculateFitness(target);
	//m_Generator = generator;
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
				break;
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

	m_Fitness = score;
}

DNA DNA::Crossover(DNA partner)
{
	DNA child{ DNA(m_Turns, m_Scramble) };

	int midPoint{ static_cast<int>(m_Genes.size() / 2) };

	if (m_Genes[midPoint] == '\'')
		++midPoint;

	child.m_Genes = m_Genes.substr(0, midPoint);

	if (partner.m_Genes[midPoint] == '\'')
		--midPoint;

	child.m_Genes += partner.m_Genes.substr(midPoint, partner.m_Genes.size() - midPoint);

	child.m_Cube = CubeState();
	child.m_Cube.Scramble(child.m_Scramble);
	//try to solve with genes
	child.m_Cube.Scramble(child.m_Genes);

	return child;
}

void DNA::Mutate(float mutationRate)
{
	char rotations[] { 'F', 'B', 'U', 'D', 'L', 'R' };
	std::string newGenes{ m_Genes };
	
	std::uniform_int_distribution<unsigned int> distRotations(0, 5);
	std::uniform_int_distribution<unsigned int> distRotationsAdditionals(0, 6);

	for (int index{}; index < m_Genes.length(); ++index)
	{
		if (static_cast<float>(rand()) / RAND_MAX < mutationRate)
		{
			if (newGenes[index] != '\'')
				newGenes[index] = rotations[distRotations(m_Generator)];
			else
			{
				char rotationsAdditional[]{ 'F', 'B', 'U', 'D', 'L', 'R', '\'' };
				if (index < newGenes.size() - 1 && newGenes[index - 1] != '\'' && newGenes[index + 1] != '\'')
					newGenes[index] = rotationsAdditional[distRotationsAdditionals(m_Generator)];
			}
		}
	}

	m_Cube = CubeState();
	m_Genes = newGenes;
	m_Cube.Scramble(m_Scramble);
	m_Cube.Scramble(m_Genes);
}