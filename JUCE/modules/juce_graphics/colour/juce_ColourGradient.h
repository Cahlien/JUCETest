/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    Describes the layout and colours that should be used to paint a colour gradient.

    @see Graphics::setGradientFill

    @tags{Graphics}
*/
class JUCE_API  ColourGradient  final
{
public:
    /** Creates an uninitialised gradient.

        If you use this constructor instead of the other one, be sure to set all the
        object's public member variables before using it!
    */
    ColourGradient() noexcept;

    ColourGradient (const ColourGradient&);
    ColourGradient (ColourGradient&&) noexcept;
    ColourGradient& operator= (const ColourGradient&);
    ColourGradient& operator= (ColourGradient&&) noexcept;

    //==============================================================================
    /** Creates a gradient object.

        (x1, y1) is the location to draw with colour1. Likewise (x2, y2) is where
        colour2 should be. In between them there's a gradient.

        If isRadial is true, the colours form a circular gradient with (x1, y1) at
        its centre.

        The alpha transparencies of the colours are used, so note that
        if you blend from transparent to a solid colour, the RGB of the transparent
        colour will become visible in parts of the gradient. e.g. blending
        from Colour::transparentBlack to Colours::white will produce a
        muddy grey colour midway, but Colour::transparentWhite to Colours::white
        will be white all the way across.

        @see ColourGradient
    */
    ColourGradient (Colour colour1, float x1, float y1,
                    Colour colour2, float x2, float y2,
                    bool isRadial);

    /** Creates a gradient object.

        point1 is the location to draw with colour1. Likewise point2 is where
        colour2 should be. In between them there's a gradient.

        If isRadial is true, the colours form a circular gradient with point1 at
        its centre.

        The alpha transparencies of the colours are used, so note that
        if you blend from transparent to a solid colour, the RGB of the transparent
        colour will become visible in parts of the gradient. e.g. blending
        from Colour::transparentBlack to Colours::white will produce a
        muddy grey colour midway, but Colour::transparentWhite to Colours::white
        will be white all the way across.

        @see ColourGradient
    */
    ColourGradient (Colour colour1, Point<float> point1,
                    Colour colour2, Point<float> point2,
                    bool isRadial);

    //==============================================================================
    /** Creates a vertical linear gradient between two Y coordinates */
    static ColourGradient vertical (Colour colour1, float y1,
                                    Colour colour2, float y2);

    /** Creates a horizontal linear gradient between two X coordinates */
    static ColourGradient horizontal (Colour colour1, float x1,
                                      Colour colour2, float x2);

    /** Creates a vertical linear gradient from top to bottom in a rectangle */
    template <typename Type>
    static ColourGradient vertical (Colour colourTop, Colour colourBottom, Rectangle<Type> area)
    {
        return vertical (colourTop, (float) area.getY(), colourBottom, (float) area.getBottom());
    }

    /** Creates a horizontal linear gradient from right to left in a rectangle */
    template <typename Type>
    static ColourGradient horizontal (Colour colourLeft, Colour colourRight, Rectangle<Type> area)
    {
        return horizontal (colourLeft, (float) area.getX(), colourRight, (float) area.getRight());
    }

    //==============================================================================
    /** Removes any colours that have been added.

        This will also remove any start and end colours, so the gradient won't work. You'll
        need to add more colours with addColour().
    */
    void clearColours();

    /** Adds a colour at a point along the length of the gradient.

        This allows the gradient to go through a spectrum of colours, instead of just a
        start and end colour.

        @param proportionAlongGradient      a value between 0 and 1.0, which is the proportion
                                            of the distance along the line between the two points
                                            at which the colour should occur.
        @param colour                       the colour that should be used at this point
        @returns the index at which the new point was added
    */
    int addColour (double proportionAlongGradient, Colour colour);

    /** Removes one of the colours from the gradient. */
    void removeColour (int index);

    /** Multiplies the alpha value of all the colours by the given scale factor */
    void multiplyOpacity (float multiplier) noexcept;

    //==============================================================================
    /** Returns the number of colour-stops that have been added. */
    int getNumColours() const noexcept;

    /** Returns the position along the length of the gradient of the colour with this index.

        The index is from 0 to getNumColours() - 1. The return value will be between 0.0 and 1.0
    */
    double getColourPosition (int index) const noexcept;

    /** Returns the colour that was added with a given index.
        The index is from 0 to getNumColours() - 1.
    */
    Colour getColour (int index) const noexcept;

    /** Changes the colour at a given index.
        The index is from 0 to getNumColours() - 1.
    */
    void setColour (int index, Colour newColour) noexcept;

    /** Returns the an interpolated colour at any position along the gradient.
        @param position     the position along the gradient, between 0 and 1
    */
    Colour getColourAtPosition (double position) const noexcept;

    //==============================================================================
    /** Creates a set of interpolated premultiplied ARGB values.
        This will resize the HeapBlock, fill it with the colours, and will return the number of
        colours that it added.
        When calling this, the ColourGradient must have at least 2 colour stops specified.
    */
    int createLookupTable (const AffineTransform& transform, HeapBlock<PixelARGB>& resultLookupTable) const;

    /** Creates a set of interpolated premultiplied ARGB values.
        This will fill an array of a user-specified size with the gradient, interpolating to fit.
        The numEntries argument specifies the size of the array, and this size must be greater than zero.
        When calling this, the ColourGradient must have at least 2 colour stops specified.
    */
    void createLookupTable (PixelARGB* resultLookupTable, int numEntries) const noexcept;

    /** Creates a set of interpolated premultiplied ARGB values.
        This will fill an array of a user-specified size with the gradient, interpolating to fit.
        When calling this, the ColourGradient must have at least 2 colour stops specified.
    */
    template <size_t NumEntries>
    void createLookupTable (PixelARGB (&resultLookupTable)[NumEntries]) const noexcept
    {
        static_assert (NumEntries != 0);
        createLookupTable (resultLookupTable, NumEntries);
    }

    /** Returns true if all colours are opaque. */
    bool isOpaque() const noexcept;

    /** Returns true if all colours are completely transparent. */
    bool isInvisible() const noexcept;

    //==============================================================================
    Point<float> point1, point2;

    /** If true, the gradient should be filled circularly, centred around
        point1, with point2 defining a point on the circumference.

        If false, the gradient is linear between the two points.
    */
    bool isRadial;

    bool operator== (const ColourGradient&) const noexcept;
    bool operator!= (const ColourGradient&) const noexcept;

    /** This comparison, and the other ordered comparisons are provided only for compatibility with
        ordered container types like std::set and std::map.
    */
    bool operator<  (const ColourGradient&) const noexcept;
    bool operator<= (const ColourGradient&) const noexcept;
    bool operator>  (const ColourGradient&) const noexcept;
    bool operator>= (const ColourGradient&) const noexcept;

private:
    //==============================================================================
    struct ColourPoint
    {
        auto tie() const { return std::tuple (position, colour.getPixelARGB().getNativeARGB()); }

        bool operator== (ColourPoint other) const noexcept { return tie() == other.tie(); }
        bool operator!= (ColourPoint other) const noexcept { return tie() != other.tie(); }
        bool operator<  (ColourPoint other) const noexcept { return tie() <  other.tie(); }

        double position;
        Colour colour;
    };

    struct ColourPointArrayComparisons;

    auto tie() const;

    Array<ColourPoint> colours;

    JUCE_LEAK_DETECTOR (ColourGradient)
};

} // namespace juce
