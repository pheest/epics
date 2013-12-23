/*bitSetUtil.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#define epicsExportSharedSymbols
#include <pv/noDefaultMethods.h>
#include <pv/pvData.h>
#include <pv/bitSetUtil.h>

namespace epics { namespace pvData {

using std::tr1::static_pointer_cast;
using std::size_t;

static bool checkBitSetPVField(
    PVFieldPtr const &pvField,BitSetPtr const &bitSet,int32 initialOffset)
{
    int32 offset = initialOffset;
    int32 nbits = pvField->getNumberFields();
    if(nbits==1) return bitSet->get(offset);
    int32 nextSetBit = bitSet->nextSetBit(offset);
    if(nextSetBit>=(offset+nbits)) return false;
    if(nextSetBit<0) return false;
    if(bitSet->get(offset)) {
        if(nbits>1) {
            for(int32 i=offset+1; i<offset+nbits; i++) bitSet->clear(i);
        }
        return true;
    }

    bool atLeastOneBitSet = false;
    bool allBitsSet = true;
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    offset = pvStructure->getFieldOffset() + 1;
    while(offset<initialOffset + nbits) {
        PVFieldPtr pvSubField = pvStructure->getSubField(offset);
        int32 nbitsNow = pvSubField->getNumberFields();
        if(nbitsNow==1) {
            if(bitSet->get(offset)) {
                atLeastOneBitSet = true;
            } else {
                allBitsSet = false;
            }
            offset++;
        } else {
            bool result = checkBitSetPVField(pvSubField,bitSet,offset);
            if(result) {
                atLeastOneBitSet = true;
                if(!bitSet->get(offset)) {
                    allBitsSet = false;
                }
            } else {
                allBitsSet = false;
            }
            offset += pvSubField->getNumberFields();
        }
    }
    if(allBitsSet) {
        if(nbits>1) {
            for(int32 i=initialOffset+1; i<initialOffset+nbits; i++){
                bitSet->clear(i);
            }
        }
        bitSet->set(initialOffset);
    }
    return atLeastOneBitSet;
}

bool BitSetUtil::compress(BitSetPtr const &bitSet,PVStructurePtr const &pvStructure)
{
    return checkBitSetPVField(pvStructure,bitSet,0);   
}

}}
