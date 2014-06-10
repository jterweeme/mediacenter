/*
Jasper ter Weeme
*/

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <iomanip>

#ifdef __GNUC__
#include <endian.h>
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

struct Header
{
    uint32_t chunkID;
    uint32_t chunkSize;
    uint16_t formatType;
    uint16_t tracks;
    uint16_t timeDivision;
} PACKED;

class MyException
{
private:
    const char *msg;
public:
    MyException() { }
    MyException(const char *msg) : msg(msg) { }
    const char *toString() { return msg; }
};

class Utility
{
public:
    static inline uint32_t be_32_toh(uint32_t x);
    static inline uint16_t be_16_toh(uint16_t x);
    static void hex(uint8_t *data, size_t len);
};

class Event
{
public:
    uint8_t eventID;
    Event() { }
    Event(uint8_t id) : eventID(id) { }

    virtual std::string toString()
    {
        std::ostringstream ss;
        ss << "Unknown Event (0x" << std::hex << std::setw(2);
        ss << std::setfill('0') << (int)eventID << ")";
        return ss.str();
    }
};

class ProgramChange : public Event
{
public:
    static const uint8_t ID = 0x12;
    std::string toString() { return "ProgramChange Event"; }
};

class ChannelAftertouch : public Event
{
public:
    static const uint8_t ID = 0x13;
    std::string toString() { return "ChannelAftertouch"; }
};

class MetaEvent : public Event
{
public:
    uint8_t metaTypeID;
    size_t length;
    MetaEvent() { }
    MetaEvent(uint8_t id) : metaTypeID(id) { }
    MetaEvent(uint8_t id, size_t length) : metaTypeID(id), length(length) { }
    static const uint8_t ID = 0xff;

    virtual std::string toString()
    {
        std::ostringstream ss;
        ss << "Unknown MetaEvent (0x" << std::hex;
        ss << std::setw(2) << std::setfill('0') << (int)metaTypeID << ")";
        return ss.str();
    }
};

class EventVector : public std::vector<Event *>
{
public:
    std::string toString();
};

class TextEvent : public MetaEvent
{
public:
    static const uint8_t ID = 1;
    char *text;
    TextEvent(size_t length) : MetaEvent(ID, length) { text = new char[length + 1]; }
    std::string toString();
};

class CopyrightNoticeEvent : public MetaEvent
{
public:
    static const uint8_t ID = 2;
    std::string toString() { return "Copyright Notice"; }
};

class TimeSignature : public MetaEvent
{
public:
    static const uint8_t ID = 0x58;
    std::string toString() { return "Time Signature"; }
};

class LyricsEvent : public MetaEvent
{
public:
    static const uint8_t ID = 5;
    std::string toString() { return "Lyrics Event"; }
};

class MarkerEvent : public MetaEvent
{
public:
    static const uint8_t ID = 6;
    std::string toString() { return "Marker Event"; }
    MarkerEvent(size_t length) : MetaEvent(ID, length) { }
};

class KeySignature : public MetaEvent
{
public:
    static const uint8_t ID = 0x59;
    std::string toString() { return "Key Signature"; }
    KeySignature(size_t length) : MetaEvent(ID, length) { }
};

class SetTempo : public MetaEvent
{
public:
    static const uint8_t ID = 0x51;
    std::string toString() { return "Set Tempo"; }
};

class CHeader
{
private:
    Header header;
public:
    CHeader() { }
    CHeader(Header header) { this->header = header; }
    CHeader(std::istream iStream) { this->read(iStream); }
    void read(std::istream &inFile);
    int getTrackCount() { return ::Utility::be_16_toh(header.tracks); }
    std::string toString();
};

class CTrack
{
private:
    uint32_t chunkIDBE;
    uint32_t chunkSizeBE;
    uint8_t *data;
    unsigned int trackNumber;
    EventVector events;
    static const uint8_t META_TAG = 0xff;
public:
    CTrack() : trackNumber(0xffff) { }
    CTrack(unsigned int n) : trackNumber(n) { }
    void read(std::istream &iStream);
    std::string toString();
    size_t getChunkSize() { return ::Utility::be_32_toh(chunkSizeBE); }
    uint8_t *getRawData() { return data; }
    void parse();
    std::string lyrics();
};

class KarFile
{
private:
    CHeader header;
    std::vector<CTrack> tracks;
 public:
    void read(std::istream &iStream);
    void dump();
    CHeader *getHeader() { return &header; }

    CTrack getTrack(unsigned int n)
    {
        if (n >= tracks.size())
            throw new MyException("Track is er niet!");

        return tracks[n];
    }
};

class KarParser1
{
public:
    int run(int argc, char **argv);
    std::string help();
};

class Options
{
private:
    int track;
    bool help;
    bool header;
    bool lyrics;
    bool events;
public:
    Options() : track(0), help(false), header(false), lyrics(false), events(false) { }
    int getTrack() { return track; }
    bool getHelp() { return help; }
    bool getHeader() { return header; }
    bool getLyrics() { return lyrics; }
    bool getEvents() { return events; }
    int parse(int argc, char **argv);
};

std::string TextEvent::toString()
{
    std::stringstream ss;
    ss << "Text Event: " << text;
    return ss.str();
}

std::string EventVector::toString()
{
    std::ostringstream ss;
    ss << "Events: " << size() << std::endl;
    int i = 0;
    
    for (std::vector<Event *>::iterator it = begin(); it != end(); ++it)
        ss << ++i << ": " << (*it)->toString() << std::endl;

    return ss.str();
}

void CTrack::parse()
{
    for (size_t i = 0; i < getChunkSize(); i++)
    {
        switch (uint8_t eventID = data[i])
        {
        case 0:
            continue;
        case ProgramChange::ID:
            events.push_back(new ProgramChange());
            break;
        case MetaEvent::ID:
        {
            switch (uint8_t metaID = data[++i])
            {
            case TextEvent::ID:
            {
                size_t length = data[++i];
                TextEvent *e = new TextEvent(length);
                
                for (size_t j = 0; j < length; j++)
                    e->text[j] = data[++i];
                    
                events.push_back(e);
            }
                break;
            case CopyrightNoticeEvent::ID:
                events.push_back(new CopyrightNoticeEvent());
                break;
            case TimeSignature::ID:
                events.push_back(new TimeSignature());
                i += 5;
                break;
            case KeySignature::ID:
            {
                size_t length = data[++i];
                events.push_back(new KeySignature(length));
                i += length;
            }
                break;
            case SetTempo::ID:
                events.push_back(new SetTempo());
                i += 4;
                break;
            case MarkerEvent::ID:
            {
                size_t length = data[++i];
                events.push_back(new MarkerEvent(length));
                i += length;
            }
                break;
            case LyricsEvent::ID:
                events.push_back(new LyricsEvent());
                break;
            default:
                Event *e = new MetaEvent(metaID);
                events.push_back(e);
                break;
            }
        }
            break;
        default:
            events.push_back(new Event(eventID));
            break;
        }
    }
}

void CHeader::read(std::istream &inFile)
{
    inFile.read((char *)&header, 14);   // moet 14 zijn!
}

void CTrack::read(std::istream &iStream)
{
    iStream.read((char *)&chunkIDBE, sizeof(chunkIDBE));
    iStream.read((char *)&chunkSizeBE, sizeof(chunkSizeBE));
    size_t chunkSize = ::Utility::be_32_toh(chunkSizeBE);
    data = new uint8_t[chunkSize];
    iStream.read((char *)data, chunkSize);
}

uint16_t Utility::be_16_toh(uint16_t x)
{
#ifdef __GNUC__
    return be16toh(x);
#endif
	return (x & 0x00ff) << 8 | (x & 0xff00) >> 8;
}

uint32_t Utility::be_32_toh(uint32_t x)
{
#ifdef __GNUC__
    return be32toh(x);
#endif
	x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
	return (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
}

std::string CTrack::toString()
{
    std::ostringstream ss;
    ss << "[Track " << trackNumber << "]" << std::endl;
    ss << "Signature: 0x" << std::hex << Utility::be_32_toh(chunkIDBE) << std::endl;
    ss << "Chunk Size: " << std::dec << Utility::be_32_toh(chunkSizeBE) << std::endl << std::endl;
    ss << events.toString();
    return ss.str();
}

std::string CHeader::toString()
{
    std::stringstream ss;
    ss << "[Header]" << std::endl;
    ss << "Signature: " << std::hex << header.chunkID << std::endl;
    ss << "Chunk Size: " << std::dec << ::Utility::be_32_toh(header.chunkSize) << std::endl;
	ss << "Format Type: " << ::Utility::be_16_toh(header.formatType) << std::endl;
	ss << "Tracks: " << ::Utility::be_16_toh(header.tracks) << std::endl;
	ss << "Time Division: " << ::Utility::be_16_toh(header.timeDivision);
    return ss.str();
}

void KarFile::read(std::istream &iStream)
{
    header.read(iStream);

    // read all tracks
    for (int i = 0; i < header.getTrackCount(); i++)
    {
        CTrack currentTrack(i);
        currentTrack.read(iStream);
        tracks.push_back(currentTrack);
    }
}

void KarFile::dump()
{
    std::cout << header.toString() << std::endl << std::endl;

    for (std::vector<CTrack>::iterator it = tracks.begin(); it != tracks.end(); ++it)
        std::cout << it->toString() << std::endl << std::endl;
}

void Utility::hex(uint8_t *data, size_t len)
{
    std::cout << std::hex;

    for (size_t i = 0; i < len; i++)
    {
        std::cout << std::hex;

        for (size_t j = i; (j < (i + 16)) && (j < len); ++j)
            std::cout << std::setw(2) << std::setfill('0') << (int)data[j] << " ";

        for (size_t j = i; (j < i + 16) && j < len; ++j)
        {
			if (data[j] < 0x20 || data[j] == 0xff)
				std::cout << ".";
            else
				std::cout << data[j];
                
        }

        std::cout << std::endl;
        i += 15;
    }
}

std::string CTrack::lyrics()
{
    std::stringstream ss;

    for (EventVector::iterator it = events.begin(); it != events.end(); ++it)
    {
        TextEvent *te = dynamic_cast<TextEvent *>(*it);
        
        if (te)
        {
            for (size_t i = 0; i < te->length; i++)
            {
                char c = te->text[i];
                
                switch (c)
                {
                case '@':   // sla dit over voor nu
                    i = te->length;
                    break;
                case '/':   // line break
                    ss << std::endl;
                    break;
                case '\\':  // paragraph break
                    ss << std::endl << std::endl;
                    break;
                default:
                    ss << te->text[i];
                    break;
                }
            }
        }
    }
    return ss.str();
}

std::string KarParser1::help()
{
    std::string s;
    s += "Usage: karparser [OPTIONS]\r\n";
    s += "\r\n";
    s += "Example: ";
    s += "  karparser -t 2 -l < song.kar\r\n";
    s += "\r\n";
    s += "  -t          Select track\r\n";
    s += "  -l          Show lyrics\r\n";
    s += "  -e          List events\r\n";
    s += "  -h, -?      This help\r\n";
    s += "  -H          Show header\r\n";
    return s;
}

int Options::parse(int argc, char **argv)
{
    if (argc <= 1)
    {
        help = true;
        return -1;
    }

    for (int i = 0; i < argc; i++)
    {
        char *opt = argv[i];
        
        if (opt[0] == '-')
        {
            switch (opt[1])
            {
            case 'h':
            case '?':
                help = true;
                return 1;       // rest van opties maken nu niet meer uit
            case 't':
                track = atoi(argv[++i]);
                break;
            case 'H':
                header = true;
                break;
            case 'l':
                lyrics = true;
                break;
            case 'e':
                events = true;
                break;
            default:
                help = true;    // rest van opties maken nu niet meer uit
                return -1;
            }
        }
    }

    return 0;
}

int KarParser1::run(int argc, char **argv)
{
    Options options;
    options.parse(argc, argv);

    if (options.getHelp())
    {
        std::cerr << help() << std::endl;
        return 0;
    }

    KarFile karFile;
    karFile.read(std::cin);

    if (options.getHeader())
        std::cout << karFile.getHeader()->toString() << std::endl << std::endl;

    try
    {
        CTrack track = karFile.getTrack(options.getTrack());
        track.parse();

        if (options.getEvents())
            std::cout << track.toString() << std::endl;

        if (options.getLyrics())
            std::cout << track.lyrics() << std::endl;
    }
    catch (MyException *e)
    {
        std::cerr << e->toString() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unkown error" << std::endl;
    }

    //uint8_t *track4data = track4.getRawData();
    //Utility::hex(track4data, track4.getChunkSize());
    return 0;
}

int main(int argc, char **argv)
{
    KarParser1 kp1;
    return kp1.run(argc, argv);
}


