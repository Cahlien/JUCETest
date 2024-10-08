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
    A small class to represent some facts about a particular type of plug-in.

    This class is for storing and managing the details about a plug-in without
    actually having to load an instance of it.

    A KnownPluginList contains a list of PluginDescription objects.

    @see KnownPluginList

    @tags{Audio}
*/
class JUCE_API  PluginDescription
{
public:
    //==============================================================================
    PluginDescription() = default;

    PluginDescription (const PluginDescription&) = default;
    PluginDescription (PluginDescription&&) = default;

    PluginDescription& operator= (const PluginDescription&) = default;
    PluginDescription& operator= (PluginDescription&&) = default;

    //==============================================================================
    /** The name of the plug-in. */
    String name;

    /** A more descriptive name for the plug-in.
        This may be the same as the 'name' field, but some plug-ins may provide an
        alternative name.
    */
    String descriptiveName;

    /** The plug-in format, e.g. "VST", "AudioUnit", etc. */
    String pluginFormatName;

    /** A category, such as "Dynamics", "Reverbs", etc. */
    String category;

    /** The manufacturer. */
    String manufacturerName;

    /** The version. This string doesn't have any particular format. */
    String version;

    /** Either the file containing the plug-in module, or some other unique way
        of identifying it.

        E.g. for an AU, this would be an ID string that the component manager
        could use to retrieve the plug-in. For a VST, it's the file path.
    */
    String fileOrIdentifier;

    /** The last time the plug-in file was changed.
        This is handy when scanning for new or changed plug-ins.
    */
    Time lastFileModTime;

    /** The last time that this information was updated. This would typically have
        been during a scan when this plugin was first tested or found to have changed.
    */
    Time lastInfoUpdateTime;

    /** Deprecated: New projects should use uniqueId instead.

        A unique ID for the plug-in.

        Note that this might not be unique between formats, e.g. a VST and some
        other format might actually have the same id.

        @see createIdentifierString
    */
    int deprecatedUid = 0;

    /** A unique ID for the plug-in.

        Note that this might not be unique between formats, e.g. a VST and some
        other format might actually have the same id.

        The uniqueId field replaces the deprecatedUid field, and fixes an issue
        where VST3 plugins with matching FUIDs would generate different uid
        values depending on the platform. The deprecatedUid field is kept for
        backwards compatibility, allowing existing hosts to migrate from the
        old uid to the new uniqueId.

        @see createIdentifierString
    */
    int uniqueId = 0;

    /** True if the plug-in identifies itself as a synthesiser. */
    bool isInstrument = false;

    /** The number of inputs. */
    int numInputChannels = 0;

    /** The number of outputs. */
    int numOutputChannels = 0;

    /** True if the plug-in is part of a multi-type container, e.g. a VST Shell. */
    bool hasSharedContainer = false;

    /** True if the plug-in is ARA enabled and can supply a valid ARAFactoryWrapper. */
    bool hasARAExtension = false;

    /** Returns true if the two descriptions refer to the same plug-in.

        This isn't quite as simple as them just having the same file (because of
        shell plug-ins).
    */
    bool isDuplicateOf (const PluginDescription& other) const noexcept;

    /** Return true if this description is equivalent to another one which created the
        given identifier string.

        Note that this isn't quite as simple as them just calling createIdentifierString()
        and comparing the strings, because the identifiers can differ (thanks to shell plug-ins).
    */
    bool matchesIdentifierString (const String& identifierString) const;

    //==============================================================================
    /** Returns a string that can be saved and used to uniquely identify the
        plugin again.

        This contains less info than the XML encoding, and is independent of the
        plug-in's file location, so can be used to store a plug-in ID for use
        across different machines.
    */
    String createIdentifierString() const;

    //==============================================================================
    /** Creates an XML object containing these details.

        @see loadFromXml
    */
    std::unique_ptr<XmlElement> createXml() const;

    /** Reloads the info in this structure from an XML record that was previously
        saved with createXML().

        Returns true if the XML was a valid plug-in description.
    */
    bool loadFromXml (const XmlElement& xml);


private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (PluginDescription)
};

} // namespace juce
