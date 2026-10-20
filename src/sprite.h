#ifndef SPRITE_H
#define SPRITE_H

#include "basic_transform.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <array>

class Sprite {  // could derive from sf::Drawable if you're willing to pay for a vptr, but not
                // necessary.
public:
    Sprite(const sf::Texture& texture)
        : mTexture(&texture),
          mVertexArray(sf::PrimitiveType::Triangles, DEFAULT_VERTICES.size()),
          mMinPointTex(0.f, 0.f),
          mMaxPointTex(static_cast<float>(texture.getSize().x),
                       static_cast<float>(texture.getSize().y)),
          mOrigin(0.f, 0.f) {}

    Sprite(const sf::Texture& texture, const sf::Vector2f& texMinPoint,
           const sf::Vector2f& texMaxPoint)
        : mTexture(&texture),
          mVertexArray(sf::PrimitiveType::Triangles, DEFAULT_VERTICES.size()),
          mMinPointTex(texMinPoint),
          mMaxPointTex(texMaxPoint),
          mOrigin(0.f, 0.f) {}

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        // The caller may have moved or resized the sprite since the last frame, so the vertices are
        // rebuilt on every draw instead of only when one of those changes.
        recomputeVertexArray();
        // The array is ours, so we hand over our texture and say that its coordinates are in
        // pixels. The caller's transform and blend mode are left alone.
        states.texture = mTexture;
        states.coordinateType = sf::CoordinateType::Pixels;
        target.draw(mVertexArray, states);
    }

    sf::Vector2f& position() { return mTransform.translation; }
    const sf::Vector2f& position() const { return mTransform.translation; }
    sf::Vector2f& scale() { return mTransform.scale; }
    const sf::Vector2f& scale() const { return mTransform.scale; }
    sf::Vector2f& origin() { return mOrigin; }
    const sf::Vector2f& origin() const { return mOrigin; }

private:
    // It may help you to write this, which is an array of 6 vertices:
    //   - the first 3 correspond to the vertices of the top-left triangle of a "default rectangle"
    //     ranging from [0, 1] in both dimensions.
    //   - the next 3 correspond to the vertices of the bottom-right triangle. of the default
    //     rectangle.
    // The rectangle is cut by the diagonal running from its top-right to its bottom-left corner, so
    // the top-left triangle keeps (0, 0) and the bottom-right one keeps (1, 1). Each triangle is
    // listed counterclockwise on screen (y down).
    static constexpr std::array<sf::Vector2f, 6> DEFAULT_VERTICES = {
        sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 1.f), sf::Vector2f(1.f, 0.f),  // top-left
        sf::Vector2f(0.f, 1.f), sf::Vector2f(1.f, 1.f), sf::Vector2f(1.f, 0.f),  // bottom-right
    };

    // if mTexture is valid, recomputes the positions and coords of sf::Vertex for each
    // of 6 possible entries in mVertexArray.
    void recomputeVertexArray() const {
        if (!mTexture)
            return;

        const sf::Vector2f texSize = mMaxPointTex - mMinPointTex;
        for (std::size_t i = 0; i < DEFAULT_VERTICES.size(); ++i) {
            // How far this corner sits across and down the sub-rectangle, in texture pixels
            const sf::Vector2f offset = DEFAULT_VERTICES[i] * texSize;
            // Measuring from the origin means the origin lands exactly on the sprite's position
            mVertexArray[i].position = mTransform.applyPoint(offset - mOrigin);
            mVertexArray[i].texCoords = mMinPointTex + offset;
        }
    }

    const sf::Texture* mTexture;
    mutable sf::VertexArray mVertexArray;  // geometry, uv coords -- mutable means it can be changed
                                           //  even in const methods.
    sf::Vector2f mMinPointTex;             // AABB for subtexture min point along both x, y
    sf::Vector2f mMaxPointTex;             // AABB for subtexture max point along both x, y
    sf::Vector2f mOrigin;  // position in tex coords of the sprite origin, relative to minPoint
    BasicTransform mTransform;
};

#endif
