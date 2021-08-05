/*
 * All the credits of this header goes to Frans Bouma (Otis_inf).
 * He provided me this header in order to have an initial library
 * to compare with regarding the performance of sapeaob.
 * Thanks a lot Otis!
 
 * It was adapted a bit by me to make it windows independant by adding some typedefs and 
 * writing the ScanPattern struct by the provided functions.
*/

#include <cstdint>
#include <string>

typedef unsigned char* LPBYTE;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

struct ScanPattern {
  ScanPattern(std::string p) : _bytePatternAsString(p){};
  void createAOBPatternFromStringPattern();

  std::size_t _patternSize;
  std::uint64_t* _bytePattern = nullptr;
  std::uint64_t* _patternMask = nullptr;
  std::string _bytePatternAsString;
  std::uint64_t _numberOf8ByteChunks;
  std::uint64_t _customOffset;
  std::uint64_t _occurrence = 1;

  std::uint64_t& occurrence() {
		return this->_occurrence;
	};
  std::uint64_t& customOffset() {
		return this->_customOffset;
	};
  std::uint64_t& numberOf8ByteChunks() {
		return this->_numberOf8ByteChunks;
	};
  std::string bytePatternAsString() {
		return this->_bytePatternAsString;
	};
  std::uint64_t*& patternMask() {
		return this->_patternMask;
	};
  std::uint64_t*& bytePattern() {
		return this->_bytePattern;
	};
  std::size_t& patternSize() {
		return this->_patternSize;
	};
};

BYTE CharToByte(char c) {
  BYTE b;
  sscanf_s(&c, "%hhx", &b);
  return b;
}

void ScanPattern::createAOBPatternFromStringPattern() {
  if (_bytePattern != nullptr) {
    // already initialized
    return;
  }
  char *pChar = (char *)_bytePatternAsString.c_str();
  _patternSize =
      _bytePatternAsString
          .size(); // this is too large, but it's not important for now. We'll
                   // correct it after the parsing of the pattern and the extra
                   // bytes allocated aren't that much.
  _numberOf8ByteChunks = (_patternSize / 8) + 1;
  _bytePattern = (uint64_t *)calloc(_numberOf8ByteChunks, sizeof(uint64_t));
  _patternMask = (uint64_t *)calloc(_numberOf8ByteChunks, sizeof(uint64_t));

  LPBYTE bytePatternAsBytes = (LPBYTE)_bytePattern;
  LPBYTE patternMaskAsBytes = (LPBYTE)_patternMask;
  _customOffset = 0;

  if (nullptr == _patternMask || nullptr == _bytePattern) {
    return;
  }

  int index = 0;
  while (*pChar) {
    // first the special characters that aren't part of bytes
    if (*pChar == ' ') {
      pChar++;
      continue;
    }

    if (*pChar == '|') {
      pChar++;
      _customOffset = index;
      continue;
    }

    // From here we handle byte related characters
    // During processing, we'll xor the two sets of 8 bytes to compare. Any bits
    // that are different will become 1. We then AND these with 1 to make them
    // stay 1. All bits we can ignore we'll AND with 0. If the end result is 0,
    // all bytes we needed to find match with the current 8 bytes
    patternMaskAsBytes[index] = 0x00;
    bytePatternAsBytes[index] = 0x00;
    if (*pChar != '?') {
      // a byte is specified
      patternMaskAsBytes[index] =
          0xFF; // we'll have to mask it with 1 so any bits that are different
                // will stand out.
      bytePatternAsBytes[index] =
          (CharToByte(pChar[0]) << 4) + CharToByte(pChar[1]);
    }
    index++;
    pChar += 2;
  }
  // index is the real size of the pattern.
  _patternSize = index;
  _numberOf8ByteChunks = (_patternSize / 8) + 1;
}



LPBYTE findAOBPattern(LPBYTE imageAddress, DWORD imageSize,
                                ScanPattern &pattern) {
  const uint64_t* bytePattern = pattern.bytePattern();
  const uint64_t* patternMask = pattern.patternMask();
  const uint8_t firstByte = ((LPBYTE)bytePattern)[0];

  LPBYTE toReturn = nullptr;
  LPBYTE currentByteAddress = imageAddress;
  const LPBYTE lastByteAddress =
      (imageAddress +
       (imageSize - (pattern.patternSize() +
                     8))); // make sure we're not scanning beyond the last byte
                           // of the image, if a byte matches in that last part
  for (int occurrence = 0; occurrence < pattern.occurrence(); occurrence++) {
    // reset the pointer found, as we're not interested in this occurrence, we
    // need a following occurrence.
    toReturn = nullptr;
    while (currentByteAddress < lastByteAddress) {
      if (*currentByteAddress == firstByte) {
        // We'll read 8 bytes at a time. Our pattern's blocks are predefined for
        // this. We'll xor the 8 byte chunks and then AND the result with the
        // masks If the result is 0, the pattern chunk matches the 8 bytes read
        // from memory and we continue to the last chunk. If not, we won't match
        // with this series of bytes and we'll continue with the next byte to
        // see if that's the start of our pattern.
        uint64_t *chunksInMemory =
            reinterpret_cast<uint64_t *>(currentByteAddress);
        bool found = true;
        for (int i = 0; i < pattern.numberOf8ByteChunks(); i++) {
          const uint64_t xorResult =
              (chunksInMemory[i] ^ bytePattern[i]) & patternMask[i];
          if (0 != xorResult) {
            // won't be a match
            found = false;
            break;
          }
        }
        if (found) {
          toReturn = currentByteAddress;
          break;
        }
      }
      currentByteAddress++;
    }
    if (nullptr == toReturn) {
      // not found, give up
      return toReturn;
    }
    currentByteAddress++; // otherwise we'll match ourselves.
  }
  return toReturn;
}
