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

FileSearchPath::FileSearchPath (const String& path)
{
    init (path);
}

FileSearchPath::FileSearchPath (const FileSearchPath& other)
   : directories (other.directories)
{
}

FileSearchPath& FileSearchPath::operator= (const FileSearchPath& other)
{
    directories = other.directories;
    return *this;
}

FileSearchPath& FileSearchPath::operator= (const String& path)
{
    init (path);
    return *this;
}

void FileSearchPath::init (const String& path)
{
    directories.clear();
    directories.addTokens (path, ";", "\"");
    directories.trim();
    directories.removeEmptyStrings();

    for (auto& d : directories)
        d = d.unquoted();
}

int FileSearchPath::getNumPaths() const
{
    return directories.size();
}

File FileSearchPath::operator[] (int index) const
{
    return File (getRawString (index));
}

String FileSearchPath::getRawString (int index) const
{
    return directories[index];
}

String FileSearchPath::toString() const
{
    return toStringWithSeparator (";");
}

String FileSearchPath::toStringWithSeparator (StringRef separator) const
{
    auto dirs = directories;

    for (auto& d : dirs)
        if (d.contains (separator))
            d = d.quoted();

    return dirs.joinIntoString (separator);
}

void FileSearchPath::add (const File& dir, int insertIndex)
{
    directories.insert (insertIndex, dir.getFullPathName());
}

bool FileSearchPath::addIfNotAlreadyThere (const File& dir)
{
    for (auto& d : directories)
        if (File (d) == dir)
            return false;

    add (dir);
    return true;
}

void FileSearchPath::remove (int index)
{
    directories.remove (index);
}

void FileSearchPath::addPath (const FileSearchPath& other)
{
    for (int i = 0; i < other.getNumPaths(); ++i)
        addIfNotAlreadyThere (other[i]);
}

void FileSearchPath::removeRedundantPaths()
{
    std::vector<String> reduced;

    for (const auto& directory : directories)
    {
        const auto checkedIsChildOf = [&] (const auto& a, const auto& b)
        {
            return File::isAbsolutePath (a) && File::isAbsolutePath (b) && File (a).isAChildOf (b);
        };

        const auto fContainsDirectory = [&] (const auto& f)
        {
            return f == directory || checkedIsChildOf (directory, f);
        };

        if (std::find_if (reduced.begin(), reduced.end(), fContainsDirectory) != reduced.end())
            continue;

        const auto directoryContainsF = [&] (const auto& f) { return checkedIsChildOf (f, directory); };

        reduced.erase (std::remove_if (reduced.begin(), reduced.end(), directoryContainsF), reduced.end());
        reduced.push_back (directory);
    }

    directories = StringArray (reduced.data(), (int) reduced.size());
}

void FileSearchPath::removeNonExistentPaths()
{
    for (int i = directories.size(); --i >= 0;)
        if (! File (directories[i]).isDirectory())
            directories.remove (i);
}

Array<File> FileSearchPath::findChildFiles (int whatToLookFor, bool recurse, const String& wildcard) const
{
    Array<File> results;
    findChildFiles (results, whatToLookFor, recurse, wildcard);
    return results;
}

int FileSearchPath::findChildFiles (Array<File>& results, int whatToLookFor,
                                    bool recurse, const String& wildcard) const
{
    int total = 0;

    for (auto& d : directories)
        total += File (d).findChildFiles (results, whatToLookFor, recurse, wildcard);

    return total;
}

bool FileSearchPath::isFileInPath (const File& fileToCheck,
                                   const bool checkRecursively) const
{
    for (auto& d : directories)
    {
        if (checkRecursively)
        {
            if (fileToCheck.isAChildOf (File (d)))
                return true;
        }
        else
        {
            if (fileToCheck.getParentDirectory() == File (d))
                return true;
        }
    }

    return false;
}

//==============================================================================
//==============================================================================
#if JUCE_UNIT_TESTS

class FileSearchPathTests final : public UnitTest
{
public:
    FileSearchPathTests() : UnitTest ("FileSearchPath", UnitTestCategories::files) {}

    void runTest() override
    {
        beginTest ("removeRedundantPaths");
        {
           #if JUCE_WINDOWS
            const String prefix = "C:";
           #else
            const String prefix = "";
           #endif

            {
                FileSearchPath fsp { prefix + "/a/b/c/d;" + prefix + "/a/b/c/e;" + prefix + "/a/b/c" };
                fsp.removeRedundantPaths();
                expectEquals (fsp.toString(), prefix + "/a/b/c");
            }

            {
                FileSearchPath fsp { prefix + "/a/b/c;" + prefix + "/a/b/c/d;" + prefix + "/a/b/c/e" };
                fsp.removeRedundantPaths();
                expectEquals (fsp.toString(), prefix + "/a/b/c");
            }

            {
                FileSearchPath fsp { prefix + "/a/b/c/d;" + prefix + "/a/b/c;" + prefix + "/a/b/c/e" };
                fsp.removeRedundantPaths();
                expectEquals (fsp.toString(), prefix + "/a/b/c");
            }

            {
                FileSearchPath fsp { "%FOO%;" + prefix + "/a/b/c;%FOO%;" + prefix + "/a/b/c/d" };
                fsp.removeRedundantPaths();
                expectEquals (fsp.toString(), "%FOO%;" + prefix + "/a/b/c");
            }
        }
    }
};

static FileSearchPathTests fileSearchPathTests;

#endif

} // namespace juce
