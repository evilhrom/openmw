#include "loadclas.hpp"

#include <stdexcept>

#include "esmreader.hpp"
#include "esmwriter.hpp"
#include "defs.hpp"
#include "util.hpp"

namespace ESM
{
    unsigned int Class::sRecordId = REC_CLAS;

    const Class::Specialization Class::sSpecializationIds[3] = {
      Class::Combat,
      Class::Magic,
      Class::Stealth
    };

    const char *Class::sGmstSpecializationIds[3] = {
      "sSpecializationCombat",
      "sSpecializationMagic",
      "sSpecializationStealth"
    };


    int& Class::CLDTstruct::getSkill (int index, bool major)
    {
        if (index<0 || index>=5)
            throw std::logic_error ("skill index out of range");

        return mSkills[index][major ? 1 : 0];
    }

    int Class::CLDTstruct::getSkill (int index, bool major) const
    {
        if (index<0 || index>=5)
            throw std::logic_error ("skill index out of range");

        return mSkills[index][major ? 1 : 0];
    }

    void Class::load(ESMReader &esm)
    {
        mId = esm.getHNString("NAME");
        if (mIsDeleted = readDeleSubRecord(esm))
        {
            return;
        }

        bool hasData = false;
        while (esm.hasMoreSubs())
        {
            esm.getSubName();
            uint32_t name = esm.retSubName().val;
            switch (name)
            {
                case ESM::FourCC<'F','N','A','M'>::value:
                    mName = esm.getHString();
                    break;
                case ESM::FourCC<'C','L','D','T'>::value:
                    esm.getHT(mData, 60);
                    if (mData.mIsPlayable > 1)
                        esm.fail("Unknown bool value");
                    hasData = true;
                    break;
                case ESM::FourCC<'D','E','S','C'>::value:
                    mDescription = esm.getHString();
                    break;
                default:
                    esm.fail("Unknown subrecord");
            }
        }
        if (!hasData)
            esm.fail("Missing CLDT subrecord");
    }
    void Class::save(ESMWriter &esm) const
    {
        esm.writeHNCString("NAME", mId);
        if (mIsDeleted)
        {
            writeDeleSubRecord(esm);
            return;
        }

        esm.writeHNOCString("FNAM", mName);
        esm.writeHNT("CLDT", mData, 60);
        esm.writeHNOString("DESC", mDescription);
    }

    void Class::blank()
    {
        mName.clear();
        mDescription.clear();

        mData.mAttribute[0] = mData.mAttribute[1] = 0;
        mData.mSpecialization = 0;
        mData.mIsPlayable = 0;
        mData.mCalc = 0;

        for (int i=0; i<5; ++i)
            for (int i2=0; i2<2; ++i2)
                mData.mSkills[i][i2] = 0;

        mIsDeleted = false;
    }
}
