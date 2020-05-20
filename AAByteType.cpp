#include "AAByteType.h"
#include "bstream.h"

unsigned char* AAByteType::ToBytes(uint32_t& size) const {

	// The binary input
	aa::bstream binaryInput;

	// Write data
	binaryInput << this->typeName;
	binaryInput << (unsigned char)this->typeVariant;
	binaryInput << this->unmanagedSize;
	binaryInput << this->baseTypePtr;
	binaryInput << this->constID;
	binaryInput << this->inheritancePtrCount;

	// Write out all inheritance pointers (that are non-base)
	for (unsigned char i = 0; i < inheritancePtrCount; i++) {
		binaryInput << otherInheritancePtr[i];
	}

	// Check if there's a vtable
	bool hasVTable = this->vtable != 0;

	// Write if there's a vtable
	binaryInput << (unsigned char)(hasVTable);

	// if has vtable, then write it
	if (hasVTable) {

		// write length
		binaryInput << (uint16_t)this->vtable->vtable.length();

		// Write all vtable elements
		this->vtable->vtable.for_each(
			[&binaryInput](_vtableelem k) {

				binaryInput << k.first;
				binaryInput << (uint16_t)k.second.length();

				k.second.for_each(
					[&binaryInput](_vtablefunc v) {

						binaryInput << v.first;
						binaryInput << v.second;

					}
				);

			}
		);

	}

	// Set output length
	size = binaryInput.length();

	// Copy array into a buffer we can return safely (It's up to the caller to deallocate 
	unsigned char* byteResult = new unsigned char[size];
	memcpy(byteResult, binaryInput.array(), size);

	// Return array
	return byteResult;

}
