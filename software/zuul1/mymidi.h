/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _MYMIDI_H_
#define _MYMIDI_H_
#include <stdint.h>
#include "misc.h"
#include "sdcard.h"

class MIDIEvent
{
public:
    uint8_t id;
    MIDIEvent() { }
    MIDIEvent(uint8_t id) : id(id) { }
    virtual ~MIDIEvent() { }
    virtual const char *toString() { return "XEvent"; }
};

class MIDIEventVector : public mstd::vector<MIDIEvent *>
{
public:
    MIDIEventVector() : mstd::vector<MIDIEvent *>(100) { }
    const char *toString();
};

class TextEvent : public MIDIEvent
{
public:
    size_t length;
    static const uint8_t ID = 1;
    char *text;
    TextEvent() { }
    TextEvent(size_t length) : MIDIEvent(ID), length(length) { text = new char[length + 1]; }
    const char *toString() { return text; }
};

class TimeSignature : public MIDIEvent
{
public:
    static const uint8_t ID = 0x58;
    TimeSignature() : MIDIEvent(ID) { }
    const char *toString() { return "Time Signature"; }
};

class KeySignature : public MIDIEvent
{
public:
    static const uint8_t ID = 0x59;
    KeySignature() : MIDIEvent(ID) { }
    const char *toString() { return "Key Signature"; }
};

class CopyRightNotice : public MIDIEvent
{
public:
    static const uint8_t ID = 2;
    CopyRightNotice() : MIDIEvent(ID) { }
    const char *toString() { return "Copyright Notice"; }
};

class KarTrack
{
public:
    uint32_t chunkIDBE;
    uint32_t chunkSizeBE;
    uint8_t *data;
    MIDIEventVector events;
    static const uint8_t META_TAG = 0xff;
    void read(MyFile *file);
    const char *toString() { return events.toString(); }
    size_t getChunkSize() { return ::Utility::be_32_toh(chunkSizeBE); }
    KarTrack() { }
    void parse();
};

struct SKarHeader
{
    uint32_t chunkIDBE;
    uint32_t chunkSizeBE;
    uint16_t formatTypeBE;
    uint16_t tracksBE;
    uint16_t timeDivisionBE;
};

class CKarHeader
{

public:
    SKarHeader header;
    CKarHeader() { }
    CKarHeader(SKarHeader header) : header(header) { }
    void read(MyFile *file);
    int getTrackCount() { return ::Utility::be_16_toh(header.tracksBE); }
    const char *toString() { return "Header"; }
};

class KarFile
{
public:
    mstd::vector<KarTrack> tracks;
    MyFile *myFile;
    uint8_t *buf;
    CKarHeader header;
public:
    KarFile(MyFile *myFile) : tracks(100), myFile(myFile) { }
    void read();
    void readToBuf();
    const char *toString();
    //const char *getText();
};


#endif

