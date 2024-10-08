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

#include "../Application/jucer_Headers.h"
#include "jucer_StoredSettings.h"
#include "../Application/jucer_Application.h"

//==============================================================================
StoredSettings& getAppSettings()
{
    return *ProjucerApplication::getApp().settings;
}

PropertiesFile& getGlobalProperties()
{
    return getAppSettings().getGlobalProperties();
}

//==============================================================================
StoredSettings::StoredSettings()
    : appearance (true),
      projectDefaults ("PROJECT_DEFAULT_SETTINGS"),
      fallbackPaths ("FALLBACK_PATHS")
{
    updateOldProjectSettingsFiles();

    reload();
    changed (true);
    flush();

    checkJUCEPaths();

    projectDefaults.addListener (this);
    fallbackPaths.addListener (this);
}

StoredSettings::~StoredSettings()
{
    projectDefaults.removeListener (this);
    fallbackPaths.removeListener (this);
    flush();
}

PropertiesFile& StoredSettings::getGlobalProperties()
{
    return *propertyFiles.getUnchecked (0);
}

static PropertiesFile* createPropsFile (const String& filename, bool isProjectSettings)
{
    return new PropertiesFile (ProjucerApplication::getApp()
                                .getPropertyFileOptionsFor (filename, isProjectSettings));
}

PropertiesFile& StoredSettings::getProjectProperties (const String& projectUID)
{
    const auto filename = String ("Projucer_Project_" + projectUID);

    for (auto i = propertyFiles.size(); --i >= 0;)
    {
        auto* const props = propertyFiles.getUnchecked (i);
        if (props->getFile().getFileNameWithoutExtension() == filename)
            return *props;
    }

    auto* p = createPropsFile (filename, true);
    propertyFiles.add (p);
    return *p;
}

void StoredSettings::updateGlobalPreferences()
{
    // update 'invisible' global settings
    updateRecentFiles();
    updateLastWizardFolder();
    updateKeyMappings();
}

void StoredSettings::updateRecentFiles()
{
    getGlobalProperties().setValue ("recentFiles", recentFiles.toString());
}

void StoredSettings::updateLastWizardFolder()
{
    getGlobalProperties().setValue ("lastWizardFolder", lastWizardFolder.getFullPathName());
}

void StoredSettings::updateKeyMappings()
{
    getGlobalProperties().removeValue ("keyMappings");

    if (auto* commandManager = ProjucerApplication::getApp().commandManager.get())
    {
        const std::unique_ptr<XmlElement> keys (commandManager->getKeyMappings()->createXml (true));

        if (keys != nullptr)
            getGlobalProperties().setValue ("keyMappings", keys.get());
    }
}

void StoredSettings::flush()
{
    updateGlobalPreferences();
    saveSwatchColours();

    for (auto i = propertyFiles.size(); --i >= 0;)
        propertyFiles.getUnchecked (i)->saveIfNeeded();
}

void StoredSettings::reload()
{
    propertyFiles.clear();
    propertyFiles.add (createPropsFile ("Projucer", false));

    if (auto projectDefaultsXml = propertyFiles.getFirst()->getXmlValue ("PROJECT_DEFAULT_SETTINGS"))
        projectDefaults = ValueTree::fromXml (*projectDefaultsXml);

    if (auto fallbackPathsXml = propertyFiles.getFirst()->getXmlValue ("FALLBACK_PATHS"))
        fallbackPaths = ValueTree::fromXml (*fallbackPathsXml);

    // recent files...
    recentFiles.restoreFromString (getGlobalProperties().getValue ("recentFiles"));
    recentFiles.removeNonExistentFiles();

    lastWizardFolder = getGlobalProperties().getValue ("lastWizardFolder");

    loadSwatchColours();
}

Array<File> StoredSettings::getLastProjects()
{
    StringArray s;
    s.addTokens (getGlobalProperties().getValue ("lastProjects"), "|", "");

    Array<File> f;
    for (int i = 0; i < s.size(); ++i)
        f.add (File (s[i]));

    return f;
}

void StoredSettings::setLastProjects (const Array<File>& files)
{
    StringArray s;
    for (int i = 0; i < files.size(); ++i)
        s.add (files.getReference (i).getFullPathName());

    getGlobalProperties().setValue ("lastProjects", s.joinIntoString ("|"));
}

void StoredSettings::updateOldProjectSettingsFiles()
{
    // Global properties file hasn't been created yet so create a dummy file
    auto projucerSettingsDirectory = ProjucerApplication::getApp().getPropertyFileOptionsFor ("Dummy", false)
                                                                  .getDefaultFile().getParentDirectory();

    auto newProjectSettingsDir = projucerSettingsDirectory.getChildFile ("ProjectSettings");
    newProjectSettingsDir.createDirectory();

    for (const auto& iter : RangedDirectoryIterator (projucerSettingsDirectory, false, "*.settings"))
    {
        auto f = iter.getFile();
        auto oldFileName = f.getFileName();

        if (oldFileName.contains ("Introjucer"))
        {
            auto newFileName = oldFileName.replace ("Introjucer", "Projucer");

            if (oldFileName.contains ("_Project"))
            {
                f.moveFileTo (f.getSiblingFile (newProjectSettingsDir.getFileName()).getChildFile (newFileName));
            }
            else
            {
                auto newFile = f.getSiblingFile (newFileName);

                // don't overwrite newer settings file
                if (! newFile.existsAsFile())
                    f.moveFileTo (f.getSiblingFile (newFileName));
            }
        }
    }
}

//==============================================================================
void StoredSettings::loadSwatchColours()
{
    swatchColours.clear();

    #define COL(col)  Colours::col,

    const Colour colours[] =
    {
        #include "../Utility/Helpers/jucer_Colours.h"
        Colours::transparentBlack
    };

    #undef COL

    const auto numSwatchColours = 24;
    auto& props = getGlobalProperties();

    for (auto i = 0; i < numSwatchColours; ++i)
        swatchColours.add (Colour::fromString (props.getValue ("swatchColour" + String (i),
                                                               colours [2 + i].toString())));
}

void StoredSettings::saveSwatchColours()
{
    auto& props = getGlobalProperties();

    for (auto i = 0; i < swatchColours.size(); ++i)
        props.setValue ("swatchColour" + String (i), swatchColours.getReference (i).toString());
}

StoredSettings::ColourSelectorWithSwatches::ColourSelectorWithSwatches() {}
StoredSettings::ColourSelectorWithSwatches::~ColourSelectorWithSwatches() {}

int StoredSettings::ColourSelectorWithSwatches::getNumSwatches() const
{
    return getAppSettings().swatchColours.size();
}

Colour StoredSettings::ColourSelectorWithSwatches::getSwatchColour (int index) const
{
    return getAppSettings().swatchColours [index];
}

void StoredSettings::ColourSelectorWithSwatches::setSwatchColour (int index, const Colour& newColour)
{
    getAppSettings().swatchColours.set (index, newColour);
}

//==============================================================================
void StoredSettings::changed (bool isProjectDefaults)
{
    std::unique_ptr<XmlElement> data (isProjectDefaults ? projectDefaults.createXml()
                                                        : fallbackPaths.createXml());

    propertyFiles.getUnchecked (0)->setValue (isProjectDefaults ? "PROJECT_DEFAULT_SETTINGS" : "FALLBACK_PATHS",
                                              data.get());
}

//==============================================================================
static bool doesSDKPathContainFile (const File& relativeTo, const String& path, const String& fileToCheckFor) noexcept
{
    auto actualPath = path.replace ("${user.home}", File::getSpecialLocation (File::userHomeDirectory).getFullPathName());
    return relativeTo.getChildFile (actualPath + "/" + fileToCheckFor).exists();
}

static bool isGlobalPathValid (const File& relativeTo, const Identifier& key, const String& path)
{
    String fileToCheckFor;

    if (key == Ids::vstLegacyPath)
    {
        fileToCheckFor = "pluginterfaces/vst2.x/aeffect.h";
    }
    else if (key == Ids::aaxPath)
    {
        fileToCheckFor = "Interfaces/AAX_Exports.cpp";
    }
    else if (key == Ids::araPath)
    {
        fileToCheckFor = "ARA_API/ARAInterface.h";
    }
    else if (key == Ids::androidSDKPath)
    {
       #if JUCE_WINDOWS
        fileToCheckFor = "platform-tools/adb.exe";
       #else
        fileToCheckFor = "platform-tools/adb";
       #endif
    }
    else if (key == Ids::defaultJuceModulePath)
    {
        fileToCheckFor = "juce_core";
    }
    else if (key == Ids::defaultUserModulePath)
    {
        fileToCheckFor = {};
    }
    else if (key == Ids::androidStudioExePath)
    {
       #if JUCE_MAC
        fileToCheckFor = "Android Studio.app";
       #elif JUCE_WINDOWS
        fileToCheckFor = "studio64.exe";
       #endif
    }
    else if (key == Ids::jucePath)
    {
        fileToCheckFor = "CHANGE_LIST.md";
    }
    else
    {
        // didn't recognise the key provided!
        jassertfalse;
        return false;
    }

    return doesSDKPathContainFile (relativeTo, path, fileToCheckFor);
}

void StoredSettings::checkJUCEPaths()
{
    auto moduleFolder = getStoredPath (Ids::defaultJuceModulePath, TargetOS::getThisOS()).get().toString();
    auto juceFolder   = getStoredPath (Ids::jucePath, TargetOS::getThisOS()).get().toString();

    auto validModuleFolder = isGlobalPathValid ({}, Ids::defaultJuceModulePath, moduleFolder);
    auto validJuceFolder   = isGlobalPathValid ({}, Ids::jucePath, juceFolder);

    if (validModuleFolder && ! validJuceFolder)
        projectDefaults.getPropertyAsValue (Ids::jucePath, nullptr) = File (moduleFolder).getParentDirectory().getFullPathName();
    else if (! validModuleFolder && validJuceFolder)
        projectDefaults.getPropertyAsValue (Ids::defaultJuceModulePath, nullptr) = File (juceFolder).getChildFile ("modules").getFullPathName();
}

bool StoredSettings::isJUCEPathIncorrect()
{
    return ! isGlobalPathValid ({}, Ids::jucePath, getStoredPath (Ids::jucePath, TargetOS::getThisOS()).get().toString());
}

static String getFallbackPathForOS (const Identifier& key, DependencyPathOS os)
{
    if (key == Ids::jucePath)
        return (os == TargetOS::windows ? "C:\\JUCE" : "~/JUCE");

    if (key == Ids::defaultJuceModulePath)
        return (os == TargetOS::windows ? "C:\\JUCE\\modules" : "~/JUCE/modules");

    if (key == Ids::defaultUserModulePath)
        return (os == TargetOS::windows ? "C:\\modules" : "~/modules");

    if (key == Ids::vstLegacyPath)
        return {};

    if (key == Ids::aaxPath)
        return {}; // Empty means "use internal SDK"

    if (key == Ids::araPath)
    {
        if (os == TargetOS::windows)  return "C:\\SDKs\\ARA_SDK";
        if (os == TargetOS::osx)      return "~/SDKs/ARA_SDK";
        return {};
    }

    if (key == Ids::androidSDKPath)
    {
        if (os == TargetOS::windows)  return "${user.home}\\AppData\\Local\\Android\\Sdk";
        if (os == TargetOS::osx)      return "${user.home}/Library/Android/sdk";
        if (os == TargetOS::linux)    return "${user.home}/Android/Sdk";

        jassertfalse;
        return {};
    }

    if (key == Ids::androidStudioExePath)
    {
        if (os == TargetOS::windows)
        {
           #if JUCE_WINDOWS
            auto path = WindowsRegistry::getValue ("HKEY_LOCAL_MACHINE\\SOFTWARE\\Android Studio\\Path", {}, {});

            if (! path.isEmpty())
                return path.unquoted() + "\\bin\\studio64.exe";
           #endif

            return "C:\\Program Files\\Android\\Android Studio\\bin\\studio64.exe";
        }

        if (os == TargetOS::osx)
            return "/Applications/Android Studio.app";

        return {}; // no Android Studio on this OS!
    }

    // unknown key!
    jassertfalse;
    return {};
}

static Identifier identifierForOS (DependencyPathOS os) noexcept
{
    if      (os == TargetOS::osx)     return Ids::osxFallback;
    else if (os == TargetOS::windows) return Ids::windowsFallback;
    else if (os == TargetOS::linux)   return Ids::linuxFallback;

    jassertfalse;
    return {};
}

ValueTreePropertyWithDefault StoredSettings::getStoredPath (const Identifier& key, DependencyPathOS os)
{
    auto tree = (os == TargetOS::getThisOS() ? projectDefaults
                                             : fallbackPaths.getOrCreateChildWithName (identifierForOS (os), nullptr));

    return { tree, key, nullptr, getFallbackPathForOS (key, os) };
}

void StoredSettings::addProjectDefaultsListener (ValueTree::Listener& l)     { projectDefaults.addListener (&l); }
void StoredSettings::removeProjectDefaultsListener (ValueTree::Listener& l)  { projectDefaults.removeListener (&l); }

void StoredSettings::addFallbackPathsListener (ValueTree::Listener& l)       { fallbackPaths.addListener (&l); }
void StoredSettings::removeFallbackPathsListener (ValueTree::Listener& l)    { fallbackPaths.removeListener (&l); }
