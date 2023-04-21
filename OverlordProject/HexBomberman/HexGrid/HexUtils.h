#pragma once

enum HexDirection
{
    NE, E, SE, SW, W, NW
};

namespace HexDirectionExtensions
{
	inline HexDirection Opposite(HexDirection direction)
    {
        return static_cast<HexDirection>(direction < 3 ? (direction + 3) : (direction - 3));
    }
}