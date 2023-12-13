#ifndef RUBIKSCUBE_HPP
#define RUBIKSCUBE_HPP
#define USING_BOOST_SERIALIZATION
#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <relearn.hpp>
#endif

//this is for a 2x2x2 cube

enum class Color
{
	yellow = 0,
	white = 2,
	green = 4,
	blue = 8,
	orange = 16,
	red = 32
};

std::string ColorToString(Color color)
{
	switch (color)
	{
	case Color::yellow:
		return "yellow";
	case Color::white:
		return "white";
	case Color::green:
		return "green";
	case Color::blue:
		return "blue";
	case Color::orange:
		return "orange";
	case Color::red:
		return "red";
	}

	return "";
}

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
	CubeActionPosibilities action;

	bool clockWise;

	std::size_t hash() const
	{
		size_t seed = 0;
		relearn::hash_combine(seed, std::hash<int>()(static_cast<int>(action)));
		return seed;
	}

	bool operator==(const CubeAction& rhs) const
	{
		return action == rhs.action;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & action;
		ar & clockWise;
	}
};

struct Piece
{
	//see cube2x2x2.png to see which side of the piece is what
	Color side1;
	Color side2;
	Color side3;

	void print() const
	{
		std::cout << '\n'
			<< ColorToString(side1) << '\n'
			<< ColorToString(side2) << '\n'
			<< ColorToString(side3) << '\n';
	}

	bool operator==(const Piece rhs) const
	{
		return this->side1 == rhs.side1 &&
			this->side2 == rhs.side2 &&
			this->side3 == rhs.side3;
	}

	bool operator!=(const Piece rhs) const
	{
		return this->side1 != rhs.side1 &&
			this->side2 != rhs.side2 &&
			this->side3 != rhs.side3;
	}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & side1;
		ar & side2;
		ar & side3;
	}
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
	static std::unique_ptr<CubeState> solvedState;
	static bool isCreatingSolvedState;

	std::vector<Piece> pieces;

	std::vector<CubeActionPosibilities> scramble; //the scramble used to get to the first state of this CubeState

	CubeState()
	{
		if (!solvedState && !isCreatingSolvedState)
		{
			isCreatingSolvedState = true;
			solvedState = std::make_unique<CubeState>();
		}

		//add all the pieces in a their correct place
		pieces.push_back({ Color::green, Color::red, Color::yellow });
		pieces.push_back({ Color::orange, Color::green, Color::yellow });
		pieces.push_back({ Color::red, Color::green, Color::white });
		pieces.push_back({ Color::green, Color::orange, Color::white });
		pieces.push_back({ Color::red, Color::blue, Color::yellow });
		pieces.push_back({ Color::blue, Color::orange, Color::yellow });
		pieces.push_back({ Color::blue, Color::red, Color::white });
		pieces.push_back({ Color::orange, Color::blue, Color::white });
	}

	CubeState(bool scramble)
		: CubeState()
	{
		if (scramble)
		{
			Scramble();
		}
	}

	void Scramble()
	{
		scramble.clear();

		//the secondTLastAction and the lastAction are random at the start to make possible for a scramble to start with rotateRightCW
		CubeActionPosibilities secondToLastAction{ GetRandomAction() };
		CubeActionPosibilities lastAction{ GetRandomAction() };
		CubeActionPosibilities action{};

		//do 20 random actions to scramble the cube
		for(int index{}; index < 20; ++index)
		{
			action = GetRandomAction();

			//this is to make sure the same action is not repeated 3 times after each other and that the same action but in the other direction is also not done after each other
			while (action == lastAction && action == secondToLastAction || AreOppositeActions(action, lastAction))
				action = GetRandomAction();
			
			scramble.push_back(action);

			secondToLastAction = lastAction;
			lastAction = action;

			DoAction(action);
		}

		if(pieces == solvedState.get()->pieces)
			Scramble();

		//PrintScramble();
	}

	static CubeActionPosibilities GetRandomAction()
	{
		//random nr in range [0, 12] to determine random action 
		//the reason why the random number is used as an exponent is that the CubeActionPosibilities has to be a power of 2 for serialization
		auto test = pow(2, rand() % 11 + 1); //+1 so the exponent cannot be 0
		auto randomNr = static_cast<CubeActionPosibilities>(test);
		return randomNr;
	}

	void PrintScramble()
	{
		std::string scrambleString{};

		for (auto action : scramble)
		{
			scrambleString += ToString(action);
		}
	}

	std::string ToString(CubeActionPosibilities action)
	{
		std::string castedMove{};

		switch (action)
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

	void DoAction(CubeActionPosibilities action)
	{
		switch (action)
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
	}

	void RotateRight(bool clockWise)
	{
		Piece tempPiece = pieces[1];

		if (clockWise)
		{
			pieces[1] = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = tempPiece;
		}
		else
		{
			tempPiece = pieces[5];
			pieces[5] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;
		}
	}

	void RotateLeft(bool clockWise)
	{
		Piece tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[4];
			pieces[4] = pieces[6];
			pieces[6] = pieces[2];
			pieces[2] = tempPiece;
		}
		else
		{
			tempPiece = pieces[2];
			pieces[2] = pieces[6];
			pieces[6] = pieces[4];
			pieces[4] = pieces[0];
			pieces[0] = tempPiece;
		}
	}

	void RotateFront(bool clockWise)
	{
		Piece tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[2];
			pieces[2] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;
		}
		else
		{
			tempPiece = pieces[1];
			pieces[1] = pieces[3];
			pieces[3] = pieces[2];
			pieces[2] = pieces[0];
			pieces[0] = tempPiece;
		}
	}

	void RotateBack(bool clockWise)
	{
		Piece tempPiece = pieces[4];

		if (clockWise)
		{
			pieces[4] = pieces[5];
			pieces[5] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = tempPiece;
		}
		else
		{
			tempPiece = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = pieces[4];
			pieces[4] = tempPiece;
		}
	}

	void RotateTop(bool clockWise)
	{
		Piece tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[1];
			pieces[1] = pieces[5];
			pieces[5] = pieces[4];
			pieces[4] = tempPiece;
		}
		else
		{
			tempPiece = pieces[4];
			pieces[4] = pieces[5];
			pieces[5] = pieces[1];
			pieces[1] = pieces[0];
			pieces[0] = tempPiece;
		}
	}

	void RotateBottom(bool clockWise)
	{
		Piece tempPiece = pieces[2];

		if (clockWise)
		{
			pieces[2] = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = tempPiece;
		}
		else
		{
			tempPiece = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = pieces[2];
			pieces[2] = tempPiece;
		}
	}

	bool IsSolved()
	{
		if (pieces == solvedState->pieces)
			return true;

		return false;
	}

	bool operator==(const CubeState rhs) const
	{
		for (int index{}; index < pieces.size(); ++index)
		{
			if (pieces[index] != rhs.pieces[index])
				return false;
		}

		return true;
	}

	//hash for relearn
	std::size_t hash() const
	{
		std::size_t seed = 0;
		for (const auto& piece : pieces) {
			// Combine the hash of each piece into the seed
			relearn::hash_combine(seed, std::hash<Color>{}(piece.side1));
			relearn::hash_combine(seed, std::hash<Color>{}(piece.side2));
			relearn::hash_combine(seed, std::hash<Color>{}(piece.side3));
		}
		return seed;
	}

private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		for (auto piece : pieces)
			ar & piece;
	}

	bool AreOppositeActions(CubeActionPosibilities action1, CubeActionPosibilities action2)
	{
		if(abs(static_cast<int>(action1) - static_cast<int>(action2)) == 64
			|| abs(log2(static_cast<int>(action1)) - log2(static_cast<int>(action2))) == 6)
		return true;

		return false;
	}
};

namespace std
{
	template <> struct hash<CubeState>
	{
		std::size_t operator()(CubeState const& arg) const
		{
			return arg.hash();
		}
	};

	template <> struct hash<CubeAction>
	{
		std::size_t operator()(CubeAction const& arg) const
		{
			return arg.hash();
		}
	};
}