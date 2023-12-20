#ifndef RUBIKSCUBE_HPP
#define RUBIKSCUBE_HPP
//#define USING_BOOST_SERIALIZATION
#include <iostream>
#include <vector>
#include <random>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#ifdef USING_BOOST_SERIALIZATION
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#endif // USING_BOOST_SERIALIZATION

//this is for a 2x2x2 cube

enum class Color
{
	yellow = 0,
	white = 2,
	green = 4,
	blue = 8,
	orange = 16,
	red = 32,
	none = 64
};

//all these action are in clockwise so if rotateBack is used imagine the cube with the back facing towards you and turn it clockwise
//CW = clockwise, CCW = counterclockwise
enum class CubeActionPosibilities
{
	rotateRightCW = 0,
	rotateLeftCW = 2,
	rotateFrontCW = 4,
	rotateBackCW = 8,
	rotateTopCW = 16,
	rotateBottomCW = 32,
	rotateRightCCW = 64,
	rotateLeftCCW = 128,
	rotateFrontCCW = 256,
	rotateBackCCW = 512,
	rotateTopCCW = 1024,
	rotateBottomCCW = 2048
};

struct CubeAction
{
	CubeAction() = default;

	CubeAction(std::mt19937& generator)
		: action(GetRandomAction(generator).action)
	{}

	CubeAction(CubeActionPosibilities _action)
		: action{ _action }
	{}

	CubeActionPosibilities action;

	bool operator==(const CubeAction& rhs) const
	{
		return action == rhs.action;
	}

private:
#ifdef USING_BOOST_SERIALIZATION
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & action;
	}
#endif

	static CubeAction GetRandomAction(std::mt19937& generator)
	{
		std::uniform_int_distribution<unsigned int> dist(1, 11);

		//random nr in range [1, 11] to determine random action 
		//the reason why the random number is used as an exponent is that the CubeActionPosibilities has to be a power of 2 for serialization
		auto randomNr = static_cast<CubeActionPosibilities>(pow(2, dist(generator)));
		return CubeAction(randomNr);
	}
};

struct Piece
{
	Piece(const std::vector<Color>& _colors)
	{
		colors = _colors;
	}

	std::vector<Color> colors{}; //left(0), right(1), front(2), back(3), top(4), bottom(5)

	void AddRotation(float radX, float radY, float radZ)
	{
		Color temp;

		if (radX == M_PI_2)
		{
			temp = colors[2];
			colors[2] = colors[4];
			colors[4] = colors[3];
			colors[3] = colors[5];
			colors[5] = temp;
		}
		else if (radX == -M_PI_2)
		{
			temp = colors[5];
			colors[5] = colors[3];
			colors[3] = colors[4];
			colors[4] = colors[2];
			colors[2] = temp;
		}
		else if (radY == M_PI_2)
		{
			temp = colors[0];
			colors[0] = colors[3];
			colors[3] = colors[1];
			colors[1] = colors[2];
			colors[2] = temp;
		}
		else if (radY == -M_PI_2)
		{
			temp = colors[2];
			colors[2] = colors[1];
			colors[1] = colors[3];
			colors[3] = colors[0];
			colors[0] = temp;
		}
		else if (radZ == M_PI_2)
		{
			temp = colors[0];
			colors[0] = colors[4];
			colors[4] = colors[1];
			colors[1] = colors[5];
			colors[5] = temp;
		}
		else if (radZ == -M_PI_2)
		{
			temp = colors[5];
			colors[5] = colors[1];
			colors[1] = colors[4];
			colors[4] = colors[0];
			colors[0] = temp;
		}
	}

	bool operator==(const Piece rhs) const
	{
		return colors == rhs.colors;
	}

	bool operator!=(const Piece rhs) const
	{
		return colors != rhs.colors;
	}

#ifdef USING_BOOST_SERIALIZATION
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & side1;
		ar & side2;
		ar & side3;
	}
#endif
};

//the pieces are oredered like this:
//m_Pieces[0] = row 0, col 0, depth 0 = left top front
//m_Pieces[1] = row 1, col 0, depth 0 = right top front
//m_Pieces[2] = row 0, col 1, depth 0 = left bottom front
//m_Pieces[3] = row 1, col 1, depth 0 = right bottom front
//m_Pieces[4] = row 0, col 0, depth 1 = left top back
//m_Pieces[5] = row 1, col 0, depth 1 = right top back
//m_Pieces[6] = row 0, col 1, depth 1 = left bottom back
//m_Pieces[7] = row 1, col 1, depth 1 = right bottom back
//row = right or left
//col = top or bottom
//depth = front or back
//KEEP IN MIND -> GREEN = FRONT AND YELLOW = TOP

struct CubeState
{
	std::vector<std::shared_ptr<Piece>> pieces{};

	std::vector<CubeAction> scramble; //the scramble used to get to the first state of this CubeState

	double reward{};

	CubeState()
	{
		for (int x{}; x < 2; ++x)
		{
			for (int y{}; y < 2; ++y)
			{
				for(int z{}; z < 2; ++z)
				{
					const std::vector<Color> colors
					{
						(x == 0) ? Color::orange : Color::none,
						(x == 1) ? Color::red : Color::none,
						(z == 0) ? Color::green : Color::none,
						(z == 1) ? Color::blue : Color::none,
						(y == 0) ? Color::white : Color::none,
						(y == 1) ? Color::yellow : Color::none
					};
					
					pieces.push_back(std::make_shared<Piece>(colors));
				}
			}
		}
	}
	
	CubeState(bool scramble, std::mt19937& generator)
		: CubeState()
	{
		if (scramble)
		{
			Scramble(generator);
		}
	}

	CubeState(const std::vector<CubeAction>& scramble)
		: CubeState()
	{
		Scramble(scramble);
	}

	void Scramble(std::mt19937& generator, int amountOfTurns = 20)
	{
		scramble.clear();

		scramble = GenerateScramble(generator, amountOfTurns);

		Scramble(scramble);

		//PrintScramble();
	}

	void Scramble(const std::vector<CubeAction>& _scramble)
	{
		scramble = _scramble;

		for (const CubeAction& action : _scramble)
		{
			DoAction(action);
		}
	}

	std::vector<CubeAction> GenerateScramble(std::mt19937& generator, int amountOfTurns = 20)
	{
		std::vector<CubeAction> generatedScramble{};

		CubeAction secondToLastAction{ generator }; //these are randomized because otherwise a scramble would never start with rotateRightCW (the default value)
		CubeAction lastAction{ generator };
		CubeAction action{ generator };

		for (int index{}; index < amountOfTurns; ++index)
		{
			action = CubeAction(generator);

			//this is to make sure the same action is not repeated 3 times after each other and that the same action but in the other direction is also not done after each other
			while (action == lastAction && action == secondToLastAction || AreOppositeActions(action, lastAction))
				action = CubeAction(generator);

			generatedScramble.push_back(action);

			secondToLastAction = lastAction;
			lastAction = action;
		}

		return generatedScramble;
	}

	void PrintScramble()
	{
		std::string scrambleString{};

		for (auto action : scramble)
		{
			scrambleString += ToString(action);
		}
	}

	std::string ToString(CubeAction action)
	{
		std::string castedMove{};

		switch (action.action)
		{
		case CubeActionPosibilities::rotateRightCW:
			castedMove += "R";
			break;
		case CubeActionPosibilities::rotateLeftCW:
			castedMove += "L";
			break;
		case CubeActionPosibilities::rotateFrontCW:
			castedMove += "F";
			break;
		case CubeActionPosibilities::rotateBackCW:
			castedMove += "B";
			break;
		case CubeActionPosibilities::rotateTopCW:
			castedMove += "U";
			break;
		case CubeActionPosibilities::rotateBottomCW:
			castedMove += "D";
			break;
		case CubeActionPosibilities::rotateRightCCW:
			castedMove += "R'";
			break;
		case CubeActionPosibilities::rotateLeftCCW:
			castedMove += "L'";
			break;
		case CubeActionPosibilities::rotateFrontCCW:
			castedMove += "F'";
			break;
		case CubeActionPosibilities::rotateBackCCW:
			castedMove += "B'";
			break;
		case CubeActionPosibilities::rotateTopCCW:
			castedMove += "U'";
			break;
		case CubeActionPosibilities::rotateBottomCCW:
			castedMove += "D'";
			break;
		}

		return castedMove;
	}

	void DoAction(CubeAction action)
	{
		switch (action.action)
		{
		case CubeActionPosibilities::rotateRightCW:
			RotateRight(true);
			break;
		case CubeActionPosibilities::rotateLeftCW:
			RotateLeft(true);
			break;
		case CubeActionPosibilities::rotateFrontCW:
			RotateFront(true);
			break;
		case CubeActionPosibilities::rotateBackCW:
			RotateBack(true);
			break;
		case CubeActionPosibilities::rotateTopCW:
			RotateTop(true);
			break;
		case CubeActionPosibilities::rotateBottomCW:
			RotateBottom(true);
			break;
		case CubeActionPosibilities::rotateRightCCW:
			RotateRight(false);
			break;
		case CubeActionPosibilities::rotateLeftCCW:
			RotateLeft(false);
			break;
		case CubeActionPosibilities::rotateFrontCCW:
			RotateFront(false);
			break;
		case CubeActionPosibilities::rotateBackCCW:
			RotateBack(false);
			break;
		case CubeActionPosibilities::rotateTopCCW:
			RotateTop(false);
			break;
		case CubeActionPosibilities::rotateBottomCCW:
			RotateBottom(false);
			break;
		}

		reward = CalculateReward();
	}

	void RotateRight(bool clockWise)
	{
		auto tempPiece = pieces[1];

		if (clockWise)
		{
			pieces[1] = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = tempPiece;
			auto test = M_PI;
			pieces[1]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[3]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[7]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[5]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
		}
		else
		{
			tempPiece = pieces[5];
			pieces[5] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;

			pieces[1]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[3]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[7]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[5]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
		}
	}

	void RotateLeft(bool clockWise)
	{
		auto tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[4];
			pieces[4] = pieces[6];
			pieces[6] = pieces[2];
			pieces[2] = tempPiece;

			pieces[0]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[4]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[6]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
			pieces[2]->AddRotation(static_cast<float>(-M_PI_2), 0.f, 0.f);
		}
		else
		{
			tempPiece = pieces[2];
			pieces[2] = pieces[6];
			pieces[6] = pieces[4];
			pieces[4] = pieces[0];
			pieces[0] = tempPiece;

			pieces[0]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[4]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[6]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
			pieces[2]->AddRotation(static_cast<float>(M_PI_2), 0.f, 0.f);
		}
	}

	void RotateFront(bool clockWise)
	{
		auto tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[2];
			pieces[2] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;

			pieces[0]->AddRotation(0.f, 0.f, static_cast<float>(-M_PI_2));
			pieces[2]->AddRotation(0.f, 0.f, static_cast<float>(-M_PI_2));
			pieces[3]->AddRotation(0.f, 0.f, static_cast<float>(-M_PI_2));
			pieces[1]->AddRotation(0.f, 0.f, static_cast<float>(-M_PI_2));
		}
		else
		{
			tempPiece = pieces[1];
			pieces[1] = pieces[3];
			pieces[3] = pieces[2];
			pieces[2] = pieces[0];
			pieces[0] = tempPiece;

			pieces[0]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[2]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[3]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[1]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
		}
	}

	void RotateBack(bool clockWise)
	{
		auto tempPiece = pieces[4];

		if (clockWise)
		{
			pieces[4] = pieces[5];
			pieces[5] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = tempPiece;

			pieces[4]->AddRotation(0.f, 0.f,static_cast<float>(-M_PI_2));
			pieces[5]->AddRotation(0.f, 0.f,static_cast<float>(-M_PI_2));
			pieces[7]->AddRotation(0.f, 0.f,static_cast<float>(-M_PI_2));
			pieces[6]->AddRotation(0.f, 0.f,static_cast<float>(-M_PI_2));
		}
		else
		{
			tempPiece = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = pieces[4];
			pieces[4] = tempPiece;

			pieces[4]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[5]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[7]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
			pieces[6]->AddRotation(0.f, 0.f, static_cast<float>(M_PI_2));
		}
	}

	void RotateTop(bool clockWise)
	{
		auto tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[1];
			pieces[1] = pieces[5];
			pieces[5] = pieces[4];
			pieces[4] = tempPiece;

			pieces[0]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[1]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[5]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[4]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
		}
		else
		{
			tempPiece = pieces[4];
			pieces[4] = pieces[5];
			pieces[5] = pieces[1];
			pieces[1] = pieces[0];
			pieces[0] = tempPiece;

			pieces[0]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[1]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[5]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[4]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
		}
	}

	void RotateBottom(bool clockWise)
	{
		auto tempPiece = pieces[2];

		if (clockWise)
		{
			pieces[2] = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = tempPiece;

			pieces[2]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[6]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[7]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
			pieces[3]->AddRotation(0.f, static_cast<float>(-M_PI_2), 0.f);
		}
		else
		{
			tempPiece = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = pieces[2];
			pieces[2] = tempPiece;
			
			pieces[2]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[6]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[7]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
			pieces[3]->AddRotation(0.f, static_cast<float>(M_PI_2), 0.f);
		}
	}

	bool IsSolved(const CubeState& target)
	{
		const int amountOfPieces{ static_cast<int>(target.pieces.size()) };

		const int amountOfColors{ static_cast<int>(target.pieces[0]->colors.size()) };

		for (int index{}; index < amountOfPieces; ++index)
		{
			bool equal{ true };

			for (int colorIndex{}; colorIndex < amountOfColors; ++colorIndex)
			{
				if (target.pieces[index]->colors[colorIndex] != pieces[index]->colors[colorIndex])
					return false;
			}
		}

		return true;
	}

	bool operator==(const CubeState rhs) const
	{
		const int amountOfPieces{ static_cast<int>(pieces.size()) };
		for (int index{}; index < amountOfPieces; ++index)
		{
			if (pieces[index] != rhs.pieces[index])
				return false;
		}

		return true;
	}

private:
#ifdef USING_BOOST_SERIALIZATION
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		for (auto& piece : pieces)
			ar & *piece;
	}
#endif

	bool AreOppositeActions(CubeAction action1, CubeAction action2)
	{
		if(abs(static_cast<int>(action1.action) - static_cast<int>(action2.action)) == 64
			|| abs(log2(static_cast<int>(action1.action)) - log2(static_cast<int>(action2.action))) == 6)
		return true;

		return false;
	}

	double CalculateReward()
	{
		//if a piece is on the correct place reward = 1
		double rewardForCorrectPlace = 1;

		double reward{};

		/*const int amountOfPieces{ static_cast<int>(pieces.size()) };
		for (int index{}; index < amountOfPieces; ++index)
		{
			if (pieces[index] == solvedState->pieces[index])
			{
				reward += rewardForCorrectPlace;
			}
		}*/

		return reward;
	}
};

//namespace std
//{
//	template <> struct hash<CubeState>
//	{
//		std::size_t operator()(CubeState const& arg) const
//		{
//			std::size_t seed = 0;
//			for (auto& piece : arg.pieces)
//			{
//				relearn::hash_combine(seed, piece->side1);
//				relearn::hash_combine(seed, piece->side2);
//				relearn::hash_combine(seed, piece->side3);
//			}
//			return seed;
//		}
//	};
//
//	template <> struct hash<CubeAction>
//	{
//		std::size_t operator()(CubeAction const& arg) const
//		{
//			std::size_t seed = 0;
//			relearn::hash_combine(seed, arg.action);
//			return seed;
//		}
//	};
//}
#endif // RUBIKSCUBE_HPP