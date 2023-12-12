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
enum class CubeActionPosibilities
{
	rotateRight = 0,
	rotateLeft = 2,
	rotateFront = 4,
	rotateBack = 8,
	rotateTop = 16,
	rotateBottom = 32
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
		//std::cout << "New cube scramble:\n";

		//do 20 random actions to scramble the cube
		for(int index{}; index < 20; ++index)
		{
			//random nr in range [0,5] to determine random action
			int randomNr{ rand() % 6 };
			bool clockWise{ static_cast<bool>(rand() % 2) };
			//std::cout << "action: " << randomNr << " clockwise: " << clockWise << '\n';
			DoAction(static_cast<CubeActionPosibilities>(randomNr), clockWise);
		}

		if(pieces == solvedState.get()->pieces)
			Scramble();
	}

	void DoAction(CubeActionPosibilities action, bool clockWise)
	{
		switch (action)
		{
		case CubeActionPosibilities::rotateRight:
			RotateRight(clockWise);
			return;
		case CubeActionPosibilities::rotateLeft:
			RotateLeft(clockWise);
			return;
		case CubeActionPosibilities::rotateFront:
			RotateFront(clockWise);
			return;
		case CubeActionPosibilities::rotateBack:
			RotateBack(clockWise);
			return;
		case CubeActionPosibilities::rotateTop:
			RotateTop(clockWise);
			return;
		case CubeActionPosibilities::rotateBottom:
			RotateBottom(clockWise);
			return;
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