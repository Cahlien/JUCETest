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

#pragma once

#include "../Project/jucer_Project.h"

//==============================================================================
class OpenDocumentManager
{
public:
    //==============================================================================
    OpenDocumentManager();

    //==============================================================================
    class Document
    {
    public:
        virtual ~Document() = default;

        virtual bool loadedOk() const = 0;
        virtual bool isForFile (const File& file) const = 0;
        virtual bool isForNode (const ValueTree& node) const = 0;
        virtual bool refersToProject (Project& project) const = 0;
        virtual Project* getProject() const = 0;
        virtual String getName() const = 0;
        virtual String getType() const = 0;
        virtual File getFile() const = 0;
        virtual bool needsSaving() const = 0;
        virtual bool saveSyncWithoutAsking() = 0;
        virtual void saveAsync (std::function<void (bool)>) = 0;
        virtual void saveAsAsync (std::function<void (bool)>) = 0;
        virtual bool hasFileBeenModifiedExternally() = 0;
        virtual void reloadFromFile() = 0;
        virtual std::unique_ptr<Component> createEditor() = 0;
        virtual std::unique_ptr<Component> createViewer() = 0;
        virtual void fileHasBeenRenamed (const File& newFile) = 0;
        virtual String getState() const = 0;
        virtual void restoreState (const String& state) = 0;
        virtual File getCounterpartFile() const   { return {}; }
    };

    //==============================================================================
    int getNumOpenDocuments() const;
    Document* getOpenDocument (int index) const;
    void clear();

    enum class SaveIfNeeded { no, yes };

    bool canOpenFile (const File& file);
    Document* openFile (Project* project, const File& file);

    void closeDocumentAsync (Document* document, SaveIfNeeded saveIfNeeded, std::function<void (bool)> callback);
    bool closeDocumentWithoutSaving (Document* document);

    void closeAllAsync (SaveIfNeeded askUserToSave, std::function<void (bool)> callback);
    void closeAllDocumentsUsingProjectAsync (Project& project, SaveIfNeeded askUserToSave, std::function<void (bool)> callback);
    void closeAllDocumentsUsingProjectWithoutSaving (Project& project);

    void closeFileWithoutSaving (const File& f);
    bool anyFilesNeedSaving() const;

    void saveAllSyncWithoutAsking();
    void saveIfNeededAndUserAgrees (Document* doc, std::function<void (FileBasedDocument::SaveResult)>);

    void reloadModifiedFiles();
    void fileHasBeenRenamed (const File& oldFile, const File& newFile);

    //==============================================================================
    class DocumentCloseListener
    {
    public:
        DocumentCloseListener() {}
        virtual ~DocumentCloseListener() {}

        // return false to force it to stop.
        virtual bool documentAboutToClose (Document* document) = 0;
    };

    void addListener (DocumentCloseListener*);
    void removeListener (DocumentCloseListener*);

    //==============================================================================
    class DocumentType
    {
    public:
        virtual ~DocumentType() = default;

        virtual bool canOpenFile (const File& file) = 0;
        virtual Document* openFile (Project* project, const File& file) = 0;
    };

    void registerType (DocumentType* type, int index = -1);

private:
    //==============================================================================
    void closeLastDocumentUsingProjectRecursive (WeakReference<OpenDocumentManager>,
                                                 Project*,
                                                 SaveIfNeeded,
                                                 std::function<void (bool)>);

    //==============================================================================
    OwnedArray<DocumentType> types;
    OwnedArray<Document> documents;
    Array<DocumentCloseListener*> listeners;
    ScopedMessageBox messageBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenDocumentManager)
    JUCE_DECLARE_WEAK_REFERENCEABLE (OpenDocumentManager)
};

//==============================================================================
class RecentDocumentList final : private OpenDocumentManager::DocumentCloseListener
{
public:
    RecentDocumentList();
    ~RecentDocumentList() override;

    void clear();

    void newDocumentOpened (OpenDocumentManager::Document* document);

    OpenDocumentManager::Document* getCurrentDocument() const       { return previousDocs.getLast(); }

    bool canGoToPrevious() const;
    bool canGoToNext() const;

    bool contains (const File&) const;

    OpenDocumentManager::Document* getPrevious();
    OpenDocumentManager::Document* getNext();

    OpenDocumentManager::Document* getClosestPreviousDocOtherThan (OpenDocumentManager::Document* oneToAvoid) const;

    void restoreFromXML (Project& project, const XmlElement& xml);
    std::unique_ptr<XmlElement> createXML() const;

private:
    bool documentAboutToClose (OpenDocumentManager::Document*) override;

    Array<OpenDocumentManager::Document*> previousDocs, nextDocs;
};
