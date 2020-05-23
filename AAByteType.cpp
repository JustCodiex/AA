#include "AAByteType.h"
#include "AAPrimitiveType.h"
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

	// Only write this if ref type
	if (this->typeVariant == AAByteTypeVariant::Ref) {

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

		// Check if there's tag data
		bool hasTagData = this->taggedFields != 0;

		// Write if the object has tag data
		binaryInput << (unsigned char)hasTagData;

		// Only write if true
		if (hasTagData) {

			// Write length
			binaryInput << (uint16_t)this->taggedFields->fields.length();

			// Write the fields
			this->taggedFields->fields.for_each(
				[&binaryInput](AAByteTagField& f) {

					// Write basic data
					binaryInput << f.fieldId;
					binaryInput << f.ptype;

					// If refptr type, write the pointer to the type referenced
					if (f.ptype == (unsigned char)AAPrimitiveType::refptr) {
						binaryInput << f.typePtr;
					}

				}
			);

		}

	}

	// Set output length
	size = binaryInput.length();

	// Copy array into a buffer we can return safely (It's up to the caller to deallocate 
	unsigned char* byteResult = new unsigned char[size];
	memcpy(byteResult, binaryInput.array(), size);

	// Return array
	return byteResult;

}
