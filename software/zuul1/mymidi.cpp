#include "mymidi.h"

void KarFile::read()
{
    header.read(myFile);

    // read all tracks
    for (int i = 0; i < header.getTrackCount(); i++)
    {
        KarTrack currentTrack;
        currentTrack.read(myFile);
        tracks.push_back(currentTrack);
    }
}

void KarTrack::read(MyFile *file)
{
    file->fread(&chunkIDBE, sizeof(chunkIDBE), 1);
    file->fread(&chunkSizeBE, sizeof(chunkSizeBE), 1);
    size_t chunkSize = ::Utility::be_32_toh(chunkSizeBE);
    data = new uint8_t[chunkSize];
    file->fread(data, chunkSize, 1);
}

void CKarHeader::read(MyFile *file)
{
    file->fread((SKarHeader *)&header, 14, 1);
}

void KarFile::readToBuf()
{
    unsigned size = myFile->getSize();
    buf = new uint8_t[size];    // zodat de grootste erin past

    for (unsigned i = 0; i < size; i++)
        buf[i] = myFile->read();
}

const char *MIDIEventVector::toString()
{
    return "MIDIEventVector";
}

void KarTrack::parse()
{
    for (size_t i = 0; i < getChunkSize(); i++)
    {
        if (data[i] == META_TAG)
        {
            uint8_t metaID = data[++i];

            switch (metaID)
            {
            case TextEvent::ID:
            {
                size_t length = data[++i];
                TextEvent *te = new TextEvent(length);

                for (size_t j = 0; j < length; j++)
                    te->text[j] = data[++i];
                
                events.push_back(te);
            }
                break;
            case CopyRightNotice::ID:
                events.push_back(new CopyRightNotice());
                break;
            case TimeSignature::ID:
                events.push_back(new TimeSignature());
                break;
            case KeySignature::ID:
                events.push_back(new KeySignature());
                break;
            default:
                events.push_back(new MIDIEvent(metaID));
                break;
            }
        }
    }
}

const char *KarFile::toString()
{
    return header.toString();
}


