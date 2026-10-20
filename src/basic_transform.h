#ifndef BASIC_TRANSFORM_H
#define BASIC_TRANSFORM_H

#include "vector_util.h"
#include <SFML/System/Vector2.hpp>

// A translation plus a non-uniform scale. We apply the scale first, about the origin, and then
// translate: the point is stretched to its final size and only then moved into place. Doing it the
// other way round would scale the translation too, so a shape would slide as it grew.
struct BasicTransform {
    BasicTransform(const sf::Vector2f& scale = sf::Vector2f(1.f, 1.f),
                   const sf::Vector2f& translation = sf::Vector2f(0.f, 0.f))
        : scale(scale), translation(translation) {}

    // A point is a location, so it feels both the scale and the translation.
    sf::Vector2f applyPoint(const sf::Vector2f& p) const { return p * scale + translation; }

    // A vector is a direction or displacement, so translation has nothing to move; only the scale
    // applies.
    sf::Vector2f applyVector(const sf::Vector2f& p) const { return p * scale; }

    bool operator==(const BasicTransform& other) const {
        return scale == other.scale && translation == other.translation;
    }

    sf::Vector2f scale;
    sf::Vector2f translation;
};

#endif
