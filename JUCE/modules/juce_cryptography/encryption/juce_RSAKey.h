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
    RSA public/private key-pair encryption class.

    An object of this type makes up one half of a public/private RSA key pair. Use the
    createKeyPair() method to create a matching pair for encoding/decoding.

    If you need to use this class in conjunction with a compatible enc/decryption
    algorithm on a webserver, you can achieve the same thing in PHP like this:

    @code
    include ('Math/BigInteger.php');  // get this from: phpseclib.sourceforge.net

    function applyToValue ($message, $key_part1, $key_part2)
    {
        $result = new Math_BigInteger();
        $zero  = new Math_BigInteger();
        $value = new Math_BigInteger (strrev ($message), 256);
        $part1 = new Math_BigInteger ($key_part1, 16);
        $part2 = new Math_BigInteger ($key_part2, 16);

        while (! $value->equals ($zero))
        {
            $result = $result->multiply ($part2);
            list ($value, $remainder) = $value->divide ($part2);
            $result = $result->add ($remainder->modPow ($part1, $part2));
        }

        return ($result->toBytes());
    }
    @endcode

    ..or in Java with something like this:

    @code
    public class RSAKey
    {
        static BigInteger applyToValue (BigInteger value, String key_part1, String key_part2)
        {
            BigInteger result = BigInteger.ZERO;
            BigInteger part1 = new BigInteger (key_part1, 16);
            BigInteger part2 = new BigInteger (key_part2, 16);

            if (part1.equals (BigInteger.ZERO) || part2.equals (BigInteger.ZERO)
                 || value.compareTo (BigInteger.ZERO) <= 0)
                return result;

            while (! value.equals (BigInteger.ZERO))
            {
                result = result.multiply (part2);
                BigInteger[] div = value.divideAndRemainder (part2);
                value = div[0];
                result = result.add (div[1].modPow (part1, part2));
            }

            return result;
        }
    }
    @endcode

    Disclaimer: neither of the code snippets above are tested! Please let me know if you have
    any corrections for them!

    @tags{Cryptography}
*/
class JUCE_API  RSAKey
{
public:
    //==============================================================================
    /** Creates a null key object.

        Initialise a pair of objects for use with the createKeyPair() method.
    */
    RSAKey();

    /** Loads a key from an encoded string representation.

        This reloads a key from a string created by the toString() method.
    */
    explicit RSAKey (const String& stringRepresentation);

    bool operator== (const RSAKey& other) const noexcept;
    bool operator!= (const RSAKey& other) const noexcept;

    //==============================================================================
    /** Turns the key into a string representation.
        This can be reloaded using the constructor that takes a string.
    */
    String toString() const;

    /** Returns true if the object is a valid key, or false if it was created by
        the default constructor.
    */
    bool isValid() const noexcept;

    //==============================================================================
    /** Encodes or decodes a value.

        Call this on the public key object to encode some data, then use the matching
        private key object to decode it.

        Returns false if the operation couldn't be completed, e.g. if this key hasn't been
        initialised correctly.

        NOTE: This method dumbly applies this key to this data. If you encode some data
        and then try to decode it with a key that doesn't match, this method will still
        happily do its job and return true, but the result won't be what you were expecting.
        It's your responsibility to check that the result is what you wanted.
    */
    bool applyToValue (BigInteger& value) const;

    //==============================================================================
    /** Creates a public/private key-pair.

        Each key will perform one-way encryption that can only be reversed by
        using the other key.

        The numBits parameter specifies the size of key, e.g. 128, 256, 512 bit. Bigger
        sizes are more secure, but this method will take longer to execute.

        The randomSeeds parameter lets you optionally pass it a set of values with
        which to seed the random number generation, improving the security of the
        keys generated. If you supply these, make sure you provide more than 2 values,
        and the more your provide, the better the security.
    */
    static void createKeyPair (RSAKey& publicKey,
                               RSAKey& privateKey,
                               int numBits,
                               const int* randomSeeds = nullptr,
                               int numRandomSeeds = 0);


protected:
    //==============================================================================
    BigInteger part1, part2;

private:
    //==============================================================================
    static BigInteger findBestCommonDivisor (const BigInteger& p, const BigInteger& q);

    JUCE_LEAK_DETECTOR (RSAKey)
};

} // namespace juce
